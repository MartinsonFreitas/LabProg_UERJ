/*
 * Solucionador de Klotski em C (Busca em Largura - BFS)
 *
 * Especificação atendida:
 * - Ignora linhas em branco e comentários iniciados por '#'
 * - Matriz inicial: letras (peças), '.' para vazio; peça alvo em maiúsculo (ex.: 'X')
 * - Matriz final: '0' como curinga; outros caracteres devem casar exatamente
 * - Suporta tabuleiros até 10x10
 * - Grupos: lê a contagem e consome as linhas (sem usar na canonicalização por simplicidade)
 *
 * Compilação: gcc -O2 -Wall -Wextra -pedantic -o klotski klotski.c
 * Execução:   ./klotski puzzle.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXN 10
#define MAXCELLS (MAXN * MAXN)
#define HASH_BUCKETS 65536
#define QUEUE_INIT 4096
#define NODE_POOL_INIT 4096

/* ---------- Tipos ---------- */

typedef struct {
    int rows, cols;
    char cells[MAXN][MAXN];
} Board;

typedef struct {
    int rows, cols;
    char cells[MAXN][MAXN];
} Goal;

typedef struct {
    int groupCount;
    int letterToGroup[256]; // -1 se não pertence a grupo
} Groups;

typedef struct Node {
    Board board;
    struct Node *parent;
    char movePiece; // letra movida
    char moveDir;   // 'U','D','L','R'
} Node;

typedef struct {
    Node **data;
    size_t head, tail, cap;
} Queue;

typedef struct Entry {
    char *key;
    struct Entry *next;
} Entry;

typedef struct {
    Entry *buckets[HASH_BUCKETS];
} HashSet;

typedef struct {
    Node **arr;
    size_t len, cap;
} NodePool;

/* ---------- Utilidades ---------- */

static void die(const char *msg) {
    fprintf(stderr, "Erro: %s\n", msg);
    exit(EXIT_FAILURE);
}

static int is_blank_or_comment(const char *s) {
    for (; *s; ++s) {
        if (*s == '#') return 1;
        if (!isspace((unsigned char)*s)) return 0;
    }
    return 1;
}

static unsigned long djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + (unsigned long)c;
    return hash;
}

/* ---------- Fila ---------- */

static void q_init(Queue *q) {
    q->cap = QUEUE_INIT;
    q->data = (Node **)malloc(q->cap * sizeof(Node *));
    if (!q->data) die("malloc fila");
    q->head = q->tail = 0;
}

static int q_empty(const Queue *q) { return q->head == q->tail; }

static void q_push(Queue *q, Node *n) {
    size_t nextTail = (q->tail + 1) % q->cap;
    if (nextTail == q->head) {
        size_t newcap = q->cap * 2;
        Node **nd = (Node **)malloc(newcap * sizeof(Node *));
        if (!nd) die("malloc fila expandida");
        size_t i = 0;
        for (size_t p = q->head; p != q->tail; p = (p + 1) % q->cap) {
            nd[i++] = q->data[p];
        }
        free(q->data);
        q->data = nd;
        q->head = 0;
        q->tail = i;
        q->cap = newcap;
        nextTail = (q->tail + 1) % q->cap;
    }
    q->data[q->tail] = n;
    q->tail = nextTail;
}

static Node *q_pop(Queue *q) {
    if (q_empty(q)) return NULL;
    Node *n = q->data[q->head];
    q->head = (q->head + 1) % q->cap;
    return n;
}

static void q_free(Queue *q) {
    free(q->data);
    q->data = NULL;
    q->cap = q->head = q->tail = 0;
}

/* ---------- HashSet ---------- */

static void hs_init(HashSet *hs) {
    memset(hs->buckets, 0, sizeof(hs->buckets));
}

static int hs_contains(HashSet *hs, const char *key) {
    unsigned long h = djb2(key) % HASH_BUCKETS;
    for (Entry *e = hs->buckets[h]; e; e = e->next) {
        if (strcmp(e->key, key) == 0) return 1;
    }
    return 0;
}

static void hs_add(HashSet *hs, const char *key) {
    unsigned long h = djb2(key) % HASH_BUCKETS;
    Entry *e = (Entry *)malloc(sizeof(Entry));
    if (!e) die("malloc entry");
    e->key = strdup(key);
    if (!e->key) die("strdup key");
    e->next = hs->buckets[h];
    hs->buckets[h] = e;
}

static void hs_free(HashSet *hs) {
    for (size_t i = 0; i < HASH_BUCKETS; ++i) {
        Entry *e = hs->buckets[i];
        while (e) {
            Entry *n = e->next;
            free(e->key);
            free(e);
            e = n;
        }
        hs->buckets[i] = NULL;
    }
}

/* ---------- Pool de nós ---------- */

static void pool_init(NodePool *p) {
    p->cap = NODE_POOL_INIT;
    p->len = 0;
    p->arr = (Node **)malloc(p->cap * sizeof(Node *));
    if (!p->arr) die("malloc pool");
}

static void pool_add(NodePool *p, Node *n) {
    if (p->len == p->cap) {
        size_t nc = p->cap * 2;
        Node **na = (Node **)realloc(p->arr, nc * sizeof(Node *));
        if (!na) die("realloc pool");
        p->arr = na;
        p->cap = nc;
    }
    p->arr[p->len++] = n;
}

static void pool_free(NodePool *p) {
    for (size_t i = 0; i < p->len; ++i) free(p->arr[i]);
    free(p->arr);
    p->arr = NULL;
    p->len = p->cap = 0;
}

/* ---------- Parsing ---------- */

static int parse_matrix(FILE *fp, Board *b) {
    char buf[256];
    int rows = 0, cols = -1;

    while (fgets(buf, sizeof(buf), fp)) {
        size_t n = strlen(buf);
        while (n && (buf[n - 1] == '\n' || buf[n - 1] == '\r')) buf[--n] = '\0';

        if (is_blank_or_comment(buf)) {
            if (rows > 0) break; // terminou a matriz
            else continue;
        }

        // Remover espaços e validar caracteres
        char row[64];
        int ridx = 0;
        for (size_t i = 0; i < n; ++i) {
            char c = buf[i];
            if (isspace((unsigned char)c)) continue;
            if (!(isalpha((unsigned char)c) || c == '.' || c == '0')) {
                // Linha não-matriz
                if (rows > 0) goto end_matrix;
                else return 0;
            }
            row[ridx++] = c;
        }
        row[ridx] = '\0';

        if (ridx == 0) {
            if (rows > 0) break;
            else continue;
        }

        if (cols == -1) cols = ridx;
        else if (cols != ridx) die("Largura inconsistente na matriz");

        if (rows >= MAXN) die("Número de linhas excede MAXN");
        for (int c = 0; c < ridx; ++c) b->cells[rows][c] = row[c];
        rows++;
    }

end_matrix:
    if (rows == 0 || cols <= 0) return 0;
    b->rows = rows;
    b->cols = cols;
    return 1;
}

static void parse_groups(FILE *fp, Groups *gr) {
    for (int i = 0; i < 256; ++i) gr->letterToGroup[i] = -1;

    char buf[256];
    // Ler número de grupos
    while (1) {
        if (!fgets(buf, sizeof(buf), fp)) {
            gr->groupCount = 0; // sem grupos
            return;
        }
        size_t n = strlen(buf);
        while (n && (buf[n - 1] == '\n' || buf[n - 1] == '\r')) buf[--n] = '\0';
        if (is_blank_or_comment(buf)) continue;
        break;
    }

    int gcount = atoi(buf);
    if (gcount < 0) gcount = 0;
    gr->groupCount = gcount;

    // Consumir gcount linhas e mapear letras (opcional)
    for (int g = 0; g < gcount; ++g) {
        if (!fgets(buf, sizeof(buf), fp)) die("EOF ao ler linha de grupo");
        size_t n = strlen(buf);
        while (n && (buf[n - 1] == '\n' || buf[n - 1] == '\r')) buf[--n] = '\0';
        if (is_blank_or_comment(buf)) { g--; continue; }
        const char *p = buf;
        while (*p) {
            while (isspace((unsigned char)*p)) ++p;
            if (!*p) break;
            char letter = *p;
            if (isalpha((unsigned char)letter)) {
                gr->letterToGroup[(unsigned char)letter] = g;
            }
            while (*p && !isspace((unsigned char)*p)) ++p;
        }
    }
}

static void parse_input(const char *filename, Board *start, Goal *goal, Groups *gr) {
    FILE *fp = fopen(filename, "r");
    if (!fp) die("Não foi possível abrir arquivo");

    if (!parse_matrix(fp, start)) {
        fclose(fp);
        die("Falha ao ler matriz inicial");
    }

    Board goalBoard;
    if (!parse_matrix(fp, &goalBoard)) {
        fclose(fp);
        die("Falha ao ler matriz final");
    }
    goal->rows = goalBoard.rows;
    goal->cols = goalBoard.cols;
    for (int r = 0; r < goal->rows; ++r)
        for (int c = 0; c < goal->cols; ++c)
            goal->cells[r][c] = goalBoard.cells[r][c];

    parse_groups(fp, gr);
    fclose(fp);
}

/* ---------- Board e chave ---------- */

static int in_bounds(const Board *b, int r, int c) {
    return r >= 0 && r < b->rows && c >= 0 && c < b->cols;
}

static int goal_matches(const Board *b, const Goal *g) {
    if (b->rows != g->rows || b->cols != g->cols) return 0;
    for (int r = 0; r < g->rows; ++r) {
        for (int c = 0; c < g->cols; ++c) {
            char gc = g->cells[r][c];
            if (gc == '0') continue; // curinga
            if (b->cells[r][c] != gc) return 0;
        }
    }
    return 1;
}

static void board_to_string(const Board *b, char *out) {
    int k = 0;
    for (int r = 0; r < b->rows; ++r) {
        for (int c = 0; c < b->cols; ++c) out[k++] = b->cells[r][c];
        out[k++] = '\n';
    }
    out[k] = '\0';
}

/* Canonicalização básica (sem usar grupos para reduzir simetria) */
static void canonicalize_key(const Board *b, const Groups *gr, char *out) {
    (void)gr; // não usado por enquanto
    board_to_string(b, out);
}

/* ---------- Movimentos ---------- */

static int is_piece_letter(char ch) {
    return isalpha((unsigned char)ch) && ch != '0';
}

static int can_move(const Board *b, char letter, int dr, int dc) {
    for (int r = 0; r < b->rows; ++r) {
        for (int c = 0; c < b->cols; ++c) {
            if (b->cells[r][c] == letter) {
                int nr = r + dr, nc = c + dc;
                if (!in_bounds(b, nr, nc)) return 0;
                char dest = b->cells[nr][nc];
                if (dest != '.' && dest != letter) return 0;
            }
        }
    }
    return 1;
}

static void apply_move(const Board *src, Board *dst, char letter, int dr, int dc) {
    *dst = *src;
    // Limpar células da peça
    for (int r = 0; r < dst->rows; ++r)
        for (int c = 0; c < dst->cols; ++c)
            if (dst->cells[r][c] == letter)
                dst->cells[r][c] = '.';
    // Colocar nas novas posições
    for (int r = 0; r < src->rows; ++r)
        for (int c = 0; c < src->cols; ++c)
            if (src->cells[r][c] == letter)
                dst->cells[r + dr][c + dc] = letter;
}

/* ---------- Solver ---------- */

static Node *new_node(const Board *b, Node *parent, char piece, char dir) {
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n) die("malloc node");
    n->board = *b;
    n->parent = parent;
    n->movePiece = piece;
    n->moveDir = dir;
    return n;
}

static Node *solve(const Board *start, const Goal *goal, const Groups *gr, NodePool *pool) {
    Queue q; q_init(&q);
    HashSet visited; hs_init(&visited);

    Node *root = new_node(start, NULL, 0, 0);
    pool_add(pool, root);

    char keybuf[MAXCELLS + MAXN + 8];
    canonicalize_key(&root->board, gr, keybuf);
    hs_add(&visited, keybuf);
    q_push(&q, root);

    while (!q_empty(&q)) {
        Node *cur = q_pop(&q);
        if (goal_matches(&cur->board, goal)) {
            q_free(&q);
            hs_free(&visited);
            return cur;
        }

        int seen[256] = {0};
        for (int r = 0; r < cur->board.rows; ++r)
            for (int c = 0; c < cur->board.cols; ++c) {
                char ch = cur->board.cells[r][c];
                if (is_piece_letter(ch)) seen[(unsigned char)ch] = 1;
            }

        const int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        const char dirc[4] = {'U','D','L','R'};

        for (int L = 0; L < 256; ++L) {
            if (!seen[L]) continue;
            char letter = (char)L;
            for (int d = 0; d < 4; ++d) {
                int dr = dirs[d][0], dc = dirs[d][1];
                if (!can_move(&cur->board, letter, dr, dc)) continue;
                Board nxt;
                apply_move(&cur->board, &nxt, letter, dr, dc);
                canonicalize_key(&nxt, gr, keybuf);
                if (hs_contains(&visited, keybuf)) continue;
                hs_add(&visited, keybuf);
                Node *child = new_node(&nxt, cur, letter, dirc[d]);
                pool_add(pool, child);
                q_push(&q, child);
            }
        }
    }

    q_free(&q);
    hs_free(&visited);
    return NULL;
}

/* ---------- Impressão ---------- */

static void print_solution(Node *goalNode) {
    int len = 0;
    for (Node *p = goalNode; p && p->parent; p = p->parent) len++;

    Node **path = (Node **)malloc((len + 1) * sizeof(Node *));
    if (!path) die("malloc path");
    int i = len;
    for (Node *p = goalNode; p; p = p->parent) path[i--] = p;

    printf("Passos: %d\n", len);
    for (int k = 1; k <= len; ++k) {
        Node *n = path[k];
        printf("Movimento %d: peça %c %c\n", k, n->movePiece, n->moveDir);
        for (int r = 0; r < n->board.rows; ++r) {
            for (int c = 0; c < n->board.cols; ++c) putchar(n->board.cells[r][c]);
            putchar('\n');
        }
        putchar('\n');
    }
    free(path);
}

/* ---------- main ---------- */

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puzzle.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Board start;
    Goal goal;
    Groups groups;

    parse_input(argv[1], &start, &goal, &groups);

    NodePool pool;
    pool_init(&pool);

    Node *res = solve(&start, &goal, &groups, &pool);
    if (!res) {
        printf("Nenhuma solução encontrada.\n");
        pool_free(&pool);
        return EXIT_SUCCESS;
    }

    print_solution(res);
    pool_free(&pool);
    return EXIT_SUCCESS;
}
