/*
 * klotski_solver_final_fixed.c
 * Versão completa e corrigida do solucionador Klotski com parser robusto.
 *
 * Compilar:
 *   gcc -O2 -std=c11 -o klotski_solver_final_fixed klotski_solver_final_fixed.c
 *
 * Executar:
 *   ./klotski_solver_final_fixed 18passos.txt
 *
 * Observações:
 * - Parser ignora comentários (#...) exceto o marcador "#solucao" (ou "#solução") que separa tabuleiro inicial da solução.
 * - Após "#grupos" tudo é ignorado.
 * - Heurística: Manhattan simples para peça alvo.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>

/* Limites */
#define MAX_ROWS 10
#define MAX_COLS 10
#define MAX_CELLS (MAX_ROWS * MAX_COLS)
#define MAX_PIECES 26
#define HASH_SIZE 200003

/* Tipos */
typedef struct {
    char id;
    int minr, minc, maxr, maxc;
    int h, w;
} PieceInfo;

typedef struct {
    int rows, cols;
    char board[MAX_ROWS][MAX_COLS];
} Board;

typedef struct Node {
    char *state_str;
    int g;
    int f;
    struct Node *parent;
    char move_piece;
    char move_dir;
} Node;

typedef struct {
    Node **data;
    int size;
    int cap;
} MinHeap;

typedef struct HashEntry {
    char *state_str;
    int g;
    struct HashEntry *next;
} HashEntry;

/* Globals de objetivo e peças */
static PieceInfo pieces[MAX_PIECES];
static int piece_count = 0;
static char goal_board[MAX_ROWS][MAX_COLS];
static int goal_rows = 0, goal_cols = 0;
static char goal_target_id = '\0';

/* Funções utilitárias */
static void die(const char *msg) { perror(msg); exit(EXIT_FAILURE); }

static char *my_strdup(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (!p) die("malloc");
    memcpy(p, s, n);
    return p;
}

static char *board_to_str(const Board *b) {
    int n = b->rows * b->cols;
    char *s = malloc(n + 1);
    if (!s) die("malloc");
    int k = 0;
    for (int i = 0; i < b->rows; ++i) for (int j = 0; j < b->cols; ++j) s[k++] = b->board[i][j];
    s[n] = '\0';
    return s;
}

static void str_to_board(const char *s, Board *b) {
    int k = 0;
    for (int i = 0; i < b->rows; ++i) for (int j = 0; j < b->cols; ++j) b->board[i][j] = s[k++];
}

static void print_board(const Board *b) {
    for (int i = 0; i < b->rows; ++i) {
        for (int j = 0; j < b->cols; ++j) putchar(b->board[i][j]);
        putchar('\n');
    }
}

/* Heap (min por f) */
static MinHeap *heap_create(int cap) {
    MinHeap *h = malloc(sizeof(MinHeap)); if (!h) die("malloc");
    h->data = malloc(sizeof(Node*) * cap); if (!h->data) die("malloc");
    h->size = 0; h->cap = cap;
    return h;
}
static void heap_swap(Node **a, Node **b) { Node *t = *a; *a = *b; *b = t; }
static void heap_push(MinHeap *h, Node *node) {
    if (h->size >= h->cap) {
        h->cap *= 2; h->data = realloc(h->data, sizeof(Node*) * h->cap); if (!h->data) die("realloc");
    }
    int i = h->size++;
    h->data[i] = node;
    while (i > 0) {
        int p = (i - 1) / 2;
        if (h->data[p]->f <= h->data[i]->f) break;
        heap_swap(&h->data[p], &h->data[i]);
        i = p;
    }
}
static Node *heap_pop(MinHeap *h) {
    if (h->size == 0) return NULL;
    Node *ret = h->data[0];
    h->data[0] = h->data[--h->size];
    int i = 0;
    while (1) {
        int l = 2*i + 1, r = 2*i + 2, smallest = i;
        if (l < h->size && h->data[l]->f < h->data[smallest]->f) smallest = l;
        if (r < h->size && h->data[r]->f < h->data[smallest]->f) smallest = r;
        if (smallest == i) break;
        heap_swap(&h->data[i], &h->data[smallest]);
        i = smallest;
    }
    return ret;
}
static void heap_free(MinHeap *h) { free(h->data); free(h); }

/* Hash (visited) */
static HashEntry **hash_table_create(void) {
    HashEntry **t = calloc(HASH_SIZE, sizeof(HashEntry*));
    if (!t) die("calloc");
    return t;
}
static unsigned long hash_str(const char *s) {
    unsigned long h = 5381; int c;
    while ((c = (unsigned char)*s++)) h = ((h << 5) + h) + c;
    return h % HASH_SIZE;
}
static int hash_get_best(HashEntry **t, const char *s) {
    unsigned long idx = hash_str(s);
    for (HashEntry *e = t[idx]; e; e = e->next) if (strcmp(e->state_str, s) == 0) return e->g;
    return INT_MAX;
}
static void hash_put(HashEntry **t, const char *s, int g) {
    unsigned long idx = hash_str(s);
    for (HashEntry *e = t[idx]; e; e = e->next) if (strcmp(e->state_str, s) == 0) { e->g = g; return; }
    HashEntry *e = malloc(sizeof(HashEntry)); if (!e) die("malloc");
    e->state_str = my_strdup(s); e->g = g; e->next = t[idx]; t[idx] = e;
}
static void hash_free(HashEntry **t) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashEntry *e = t[i];
        while (e) { HashEntry *n = e->next; free(e->state_str); free(e); e = n; }
    }
    free(t);
}

/* Funções de parsing (corrigidas) */

/* Retorna linha não-comentário (aloca com my_strdup), ou NULL EOF.
   Retorna a string "#solucao" exatamente (como marcador) se encontrar o marcador.
   Ignora todas as linhas que começam com '#' exceto "#solucao" / "#solução". */
/* =====================================================
   FUNÇÃO: next_noncomment_line — VERSÃO FINAL
   ===================================================== */
static char *next_noncomment_line(FILE *f) {
    static char buf[2048];

    while (1) {
        if (!fgets(buf, sizeof(buf), f))
            return NULL;

        // Remove \n \r
        size_t len = strlen(buf);
        while (len > 0 && (buf[len-1] == '\n' || buf[len-1] == '\r'))
            buf[--len] = '\0';

        // Remove espaços no início
        size_t start = 0;
        while (start < len && isspace((unsigned char)buf[start]))
            start++;

        if (start >= len)
            continue; // linha vazia

        // Comentário?
        if (buf[start] == '#') {

            // detectar marcador
            if (strncmp(buf + start, "#solucao", 8) == 0 ||
                strncmp(buf + start, "#solução", 9) == 0)
            {
                return my_strdup("#solucao");
            }

            // ignorar QUALQUER outro comentário
            continue;
        }

        // Retornar linha normal
        return my_strdup(buf + start);
    }
}

/* Lê arquivo conforme formato Haikori: tabuleiro inicial, "#solucao", tabuleiro objetivo, "#grupos"... */
/* =====================================================
   FUNÇÃO: parse_input — VERSÃO FINAL
   ===================================================== */
static void parse_input(const char *filename, Board *init) {
    FILE *f = fopen(filename, "r");
    if (!f) die("fopen");

    char *lines[MAX_ROWS];
    int r = 0, maxc = 0;

    /* ---------- LEITURA DO TABULEIRO INICIAL ---------- */
    while (1) {
        char *ln = next_noncomment_line(f);
        if (!ln) die("Arquivo terminou antes de #solucao");

        if (strcmp(ln, "#solucao") == 0) {
            free(ln);
            break;
        }

        if (r >= MAX_ROWS) die("Tabuleiro inicial grande demais");

        lines[r++] = ln;
        int L = strlen(ln);
        if (L > maxc) maxc = L;
    }

    init->rows = r;
    init->cols = maxc;

    for (int i = 0; i < r; i++) {
        int L = strlen(lines[i]);
        for (int j = 0; j < maxc; j++)
            init->board[i][j] = (j < L ? lines[i][j] : '.');
        free(lines[i]);
    }

    /* ----------- LEITURA DO TABULEIRO SOLUÇÃO --------- */
    char *goal_lines[MAX_ROWS];
    int gr = 0, gmaxc = 0;

    while (1) {
        char *ln = next_noncomment_line(f);
        if (!ln) break;

        if (strncmp(ln, "#grupos", 7) == 0) {
            free(ln);
            break;
        }

        if (gr >= MAX_ROWS) die("Tabuleiro solução grande demais");

        goal_lines[gr++] = ln;
        int L = strlen(ln);
        if (L > gmaxc) gmaxc = L;
    }

    goal_rows = gr;
    goal_cols = gmaxc;

    for (int i = 0; i < gr; i++) {
        int L = strlen(goal_lines[i]);
        for (int j = 0; j < gmaxc; j++)
            goal_board[i][j] = (j < L ? goal_lines[i][j] : '0');
        free(goal_lines[i]);
    }

    fclose(f);
}

/* Detecta peças no board (flood-fill) */
static void detect_pieces(const Board *b) {
    piece_count = 0;
    bool seen[MAX_ROWS][MAX_COLS] = {0};
    for (int i = 0; i < b->rows; ++i) for (int j = 0; j < b->cols; ++j) seen[i][j] = false;

    for (int i = 0; i < b->rows; ++i) {
        for (int j = 0; j < b->cols; ++j) {
            char ch = b->board[i][j];
            if (ch == '.' || ch == '0' || ch == '\0') continue;
            if (seen[i][j]) continue;
            char id = ch;
            int minr = i, minc = j, maxr = i, maxc = j;
            int stk_r[MAX_CELLS], stk_c[MAX_CELLS], sp = 0;
            stk_r[sp] = i; stk_c[sp] = j; sp++; seen[i][j] = true;
            while (sp) {
                sp--; int r = stk_r[sp], c = stk_c[sp];
                if (r < minr) minr = r; if (c < minc) minc = c; if (r > maxr) maxr = r; if (c > maxc) maxc = c;
                const int dr[4] = {-1,1,0,0}; const int dc[4] = {0,0,-1,1};
                for (int k = 0; k < 4; ++k) {
                    int nr = r + dr[k], nc = c + dc[k];
                    if (nr < 0 || nr >= b->rows || nc < 0 || nc >= b->cols) continue;
                    if (seen[nr][nc]) continue;
                    if (b->board[nr][nc] == id) { seen[nr][nc] = true; stk_r[sp] = nr; stk_c[sp] = nc; sp++; }
                }
            }
            PieceInfo p; p.id = id; p.minr = minr; p.minc = minc; p.maxr = maxr; p.maxc = maxc; p.h = maxr - minr + 1; p.w = maxc - minc + 1;
            pieces[piece_count++] = p;
            if (isupper((unsigned char)id)) goal_target_id = id;
            if (piece_count >= MAX_PIECES) return;
        }
    }
}

/* Detecta peças em board fornecido (usada para heurística/filhos) */
static void detect_pieces_in_board(Board *b, PieceInfo out_pieces[], int *out_count) {
    bool seen[MAX_ROWS][MAX_COLS] = {0};
    int cnt = 0;
    for (int i = 0; i < b->rows; ++i) for (int j = 0; j < b->cols; ++j) seen[i][j] = false;
    for (int i = 0; i < b->rows; ++i) for (int j = 0; j < b->cols; ++j) {
        char ch = b->board[i][j];
        if (ch == '.' || ch == '0') continue;
        if (seen[i][j]) continue;
        int stk_r[MAX_CELLS], stk_c[MAX_CELLS], sp = 0;
        stk_r[sp] = i; stk_c[sp] = j; sp++; seen[i][j] = true;
        int minr = i, minc = j, maxr = i, maxc = j;
        while (sp) {
            sp--; int r = stk_r[sp], c = stk_c[sp];
            if (r < minr) minr = r; if (c < minc) minc = c; if (r > maxr) maxr = r; if (c > maxc) maxc = c;
            const int dr[4] = {-1,1,0,0}; const int dc[4] = {0,0,-1,1};
            for (int k = 0; k < 4; ++k) {
                int nr = r + dr[k], nc = c + dc[k];
                if (nr < 0 || nr >= b->rows || nc < 0 || nc >= b->cols) continue;
                if (seen[nr][nc]) continue;
                if (b->board[nr][nc] == ch) { seen[nr][nc] = true; stk_r[sp] = nr; stk_c[sp] = nc; sp++; }
            }
        }
        PieceInfo p; p.id = ch; p.minr = minr; p.minc = minc; p.maxr = maxr; p.maxc = maxc; p.h = maxr - minr + 1; p.w = maxc - minc + 1;
        out_pieces[cnt++] = p; if (cnt >= MAX_PIECES) break;
    }
    *out_count = cnt;
}

/* Heurística: distância Manhattan entre canto superior-esquerdo do bloco-alvo e posições candidatas na goal_board */
static int heuristic_board(const Board *b) {
    if (goal_target_id == '\0') return 0;
    int tr=-1, tc=-1, th=0, tw=0;
    for (int i = 0; i < piece_count; ++i) if (pieces[i].id == goal_target_id) { tr = pieces[i].minr; tc = pieces[i].minc; th = pieces[i].h; tw = pieces[i].w; break; }
    if (tr == -1) return 0;
    int best = INT_MAX;
    for (int i = 0; i <= b->rows - th; ++i) for (int j = 0; j <= b->cols - tw; ++j) {
        bool ok = true; int cost = 0;
        for (int r = 0; r < th && ok; ++r) for (int c = 0; c < tw; ++c) {
            char gch = (i+r < goal_rows && j+c < goal_cols) ? goal_board[i+r][j+c] : '0';
            if (gch == '0') continue;
            if (gch != goal_target_id) { ok = false; break; }
        }
        if (!ok) continue;
        cost = abs(tr - i) + abs(tc - j);
        if (cost < best) best = cost;
    }
    if (best == INT_MAX) return 0;
    return best;
}

/* Tenta mover peça pinfo por (dr,dc). Preenche out se válido. */
static bool try_move_piece(const Board *b, const PieceInfo *pinfo, int dr, int dc, Board *out) {
    *out = *b;
    int new_minr = pinfo->minr + dr, new_minc = pinfo->minc + dc;
    int new_maxr = pinfo->maxr + dr, new_maxc = pinfo->maxc + dc;
    if (new_minr < 0 || new_minc < 0 || new_maxr >= b->rows || new_maxc >= b->cols) return false;

    /* limpar células antigas que pertencem à peça */
    for (int r = pinfo->minr; r <= pinfo->maxr; ++r) for (int c = pinfo->minc; c <= pinfo->maxc; ++c)
        if (b->board[r][c] == pinfo->id) out->board[r][c] = '.';

    /* verificar colisões na nova posição (somente nas células que originalmente eram parte da peça) */
    for (int r = 0; r < pinfo->h; ++r) for (int c = 0; c < pinfo->w; ++c) {
        char cur = b->board[pinfo->minr + r][pinfo->minc + c];
        if (cur != pinfo->id) continue;
        int tr = pinfo->minr + r + dr;
        int tc = pinfo->minc + c + dc;
        char dest = b->board[tr][tc];
        if (dest != '.' && dest != pinfo->id) return false;
    }

    /* aplicar peça nas novas células */
    for (int r = 0; r < pinfo->h; ++r) for (int c = 0; c < pinfo->w; ++c) {
        char cur = b->board[pinfo->minr + r][pinfo->minc + c];
        if (cur != pinfo->id) continue;
        int tr = pinfo->minr + r + dr;
        int tc = pinfo->minc + c + dc;
        out->board[tr][tc] = pinfo->id;
    }
    return true;
}

/* Verifica se estado b é goal */
static bool is_goal_state(const Board *b) {
    for (int i = 0; i < goal_rows; ++i) for (int j = 0; j < goal_cols; ++j) {
        char gch = goal_board[i][j];
        if (gch == '0') continue;
        if (i >= b->rows || j >= b->cols) return false;
        if (b->board[i][j] != gch) return false;
    }
    return true;
}

/* Reconstruir solução (imprime movimentos da raiz até goal) */
static void reconstruct_and_print_solution(Node *goal_node) {
    int steps = 0;
    for (Node *n = goal_node; n && n->parent; n = n->parent) steps++;
    if (steps == 0) { printf("Solução: 0 movimentos (estado inicial é objetivo)\n"); return; }

    Node **arr = malloc(sizeof(Node*) * steps);
    if (!arr) die("malloc");
    Node *cur = goal_node;
    for (int i = steps - 1; i >= 0; --i) { arr[i] = cur; cur = cur->parent; }

    printf("Solução (%d movimentos):\n", steps);
    for (int i = 0; i < steps; ++i) {
        char piece = arr[i]->move_piece;
        char dir = arr[i]->move_dir;
        /* Mapear direção para saída (U D L R). Se quiser outro mapeamento, ajuste aqui. */
        printf("%c%c", piece ? piece : '?', dir ? dir : '?');
        if (i+1 < steps) putchar(' ');
    }
    putchar('\n');
    free(arr);
}

/* A* */
static void a_star_solve(Board *init) {
    detect_pieces(init);
    int h0 = heuristic_board(init);
    char *init_str = board_to_str(init);

    MinHeap *open = heap_create(1024);
    HashEntry **visited = hash_table_create();

    Node *start = malloc(sizeof(Node)); if (!start) die("malloc");
    start->state_str = init_str; start->g = 0; start->f = h0; start->parent = NULL; start->move_piece = '\0'; start->move_dir = '\0';
    heap_push(open, start);
    hash_put(visited, start->state_str, 0);

    int explored = 0;
    while (open->size > 0) {
        Node *cur = heap_pop(open);
        explored++;
        Board b; b.rows = init->rows; b.cols = init->cols; str_to_board(cur->state_str, &b);
        if (is_goal_state(&b)) {
            printf("Explorados: %d\n", explored);
            reconstruct_and_print_solution(cur);
            heap_free(open); hash_free(visited);
            return;
        }

        /* gerar vizinhos */
        PieceInfo cur_pieces[MAX_PIECES]; int cur_pc = 0; detect_pieces_in_board(&b, cur_pieces, &cur_pc);
        for (int pi = 0; pi < cur_pc; ++pi) {
            PieceInfo *p = &cur_pieces[pi];
            const int drs[4] = {-1,1,0,0}; const int dcs[4] = {0,0,-1,1};
            const char dirs[4] = {'U','D','L','R'};
            for (int mv = 0; mv < 4; ++mv) {
                Board nb;
                if (!try_move_piece(&b, p, drs[mv], dcs[mv], &nb)) continue;
                char *ns = board_to_str(&nb);
                int prev_g = hash_get_best(visited, ns);
                int ng = cur->g + 1;
                if (ng < prev_g) {
                    /* calcular heurística: detect pieces temporariamente */
                    PieceInfo tmp_pieces[MAX_PIECES]; int tmp_pc = 0;
                    detect_pieces_in_board(&nb, tmp_pieces, &tmp_pc);
                    int saved_pc = piece_count;
                    memcpy(pieces, tmp_pieces, sizeof(PieceInfo)*tmp_pc);
                    piece_count = tmp_pc;
                    int h = heuristic_board(&nb);
                    piece_count = saved_pc;

                    Node *nn = malloc(sizeof(Node)); if (!nn) die("malloc");
                    nn->state_str = ns; nn->g = ng; nn->f = ng + h; nn->parent = cur; nn->move_piece = p->id; nn->move_dir = dirs[mv];
                    heap_push(open, nn);
                    hash_put(visited, ns, ng);
                } else {
                    free(ns);
                }
            }
        }
        /* NOTA: não liberamos cur nem cur->state_str pois são usados na cadeia de parents para reconstrução.
           Para uma versão 'production' libere tudo usando uma lista dos nós alocados. */
    }

    printf("Solução não encontrada (A* esgotou).\n");
    heap_free(open); hash_free(visited);
}

/* main */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_puzzle.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Board init;
    parse_input(argv[1], &init);
    printf("Tabuleiro inicial (%dx%d):\n", init.rows, init.cols);
    print_board(&init);
    printf("Tabuleiro objetivo (%dx%d):\n", goal_rows, goal_cols);
    for (int i = 0; i < goal_rows; ++i) { for (int j = 0; j < goal_cols; ++j) putchar(goal_board[i][j]); putchar('\n'); }

    a_star_solve(&init);
    return 0;
}
