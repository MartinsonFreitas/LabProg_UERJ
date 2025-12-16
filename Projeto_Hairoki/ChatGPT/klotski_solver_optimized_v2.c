/*
 * klotski_solver_optimized.c
 * Versão otimizada e mais robusta do solucionador Klotski (substitui klotski_solver_final.c)
 * Estratégia: A* (busca informada) sobre estados representados por strings (tabuleiro completo).
 * - Parsing conforme enunciado Haikori2025-2
 * - Detecta peças (conjuntos conectados com mesma letra) e seus bounding boxes
 * - Gera movimentos válidos por peça (U/D/L/R) testando colisões
 * - Heurística simples: distância Manhattan do bloco-alvo até as posições alvo do tabuleiro
 * - Tabela de visitados guarda custo mínimo g já visto para cada estado
 *
 * Observações importantes (leia antes de usar):
 * - Compilar: gcc -O2 -std=c11 -o klotski_solver_optimized klotski_solver_optimized.c
 * - Execução: ./klotski_solver_optimized <arquivo_puzzle.txt>
 * - Esta versão prioriza robustez de parsing e correção. Pode ser adaptada para IDA*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

// Limites do problema (conforme enunciado)
#define MAX_ROWS 10
#define MAX_COLS 10
#define MAX_CELLS (MAX_ROWS * MAX_COLS)
#define MAX_PIECES 26
#define HASH_SIZE 200003 // primo

// Estruturas auxiliares
typedef struct {
    char id;            // letra da peça
    int minr, minc;     // bounding box
    int maxr, maxc;
    int h, w;           // altura e largura
} PieceInfo;

typedef struct {
    int rows, cols;
    char board[MAX_ROWS][MAX_COLS]; // estado atual
} Board;

// Nó para A*
typedef struct Node {
    char *state_str;    // representação linear do tabuleiro
    int g;              // custo do caminho até aqui
    int f;              // g + h
    struct Node *parent;
    char move_piece;    // peça movida para chegar aqui
    char move_dir;      // 'U','D','L','R'
} Node;

// Min-heap (priority queue) de nós por f
typedef struct {
    Node **data;
    int size;
    int cap;
} MinHeap;

// Entrada do hash (tabela de visitados)
typedef struct HashEntry {
    char *state_str;
    int g; // melhor g conhecido
    struct HashEntry *next;
} HashEntry;

// Estado global de parsing (peças e objetivo)
PieceInfo pieces[MAX_PIECES];
int piece_count = 0;
char goal_board[MAX_ROWS][MAX_COLS];
int goal_rows = 0, goal_cols = 0;
char goal_target_id = '\0'; // letra maiúscula do alvo (se houver)

// ------------------- Funções utilitárias -------------------
static void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
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

// imprime (debug)
static void print_board(const Board *b) {
    for (int i = 0; i < b->rows; ++i) {
        for (int j = 0; j < b->cols; ++j) putchar(b->board[i][j]);
        putchar('\n');
    }
}

// ------------------- Heap -------------------
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
static void heap_free(MinHeap *h) {
    free(h->data); free(h);
}

// ------------------- Hash (visited) -------------------
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
    e->state_str = strdup(s); e->g = g; e->next = t[idx]; t[idx] = e;
}
static void hash_free(HashEntry **t) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashEntry *e = t[i];
        while (e) { HashEntry *n = e->next; free(e->state_str); free(e); e = n; }
    }
    free(t);
}

// ------------------- Parsing -------------------
// Lê linhas ignorando comentários/linhas vazias
static char *next_noncomment_line(FILE *f) {
    static char buf[1024];

    while (1) {
        if (!fgets(buf, sizeof(buf), f))
            return NULL;

        // Remover \n e \r
        size_t L = strlen(buf);
        while (L > 0 && (buf[L-1] == '\n' || buf[L-1] == '\r'))
            buf[--L] = '\0';

        // Pular linhas vazias ou só com espaços
        size_t start = 0;
        while (start < L && isspace((unsigned char)buf[start]))
            start++;

        if (start >= L)
            continue; 

        // Detectar marcador "#solucao" ou "#solução"
        if (buf[start] == '#') {
            char tmp[32];
            size_t p = 0;
            size_t i = start + 1;

            while (i < L && p + 1 < sizeof(tmp) && !isspace((unsigned char)buf[i])) {
                tmp[p++] = tolower((unsigned char)buf[i]);
                i++;
            }
            tmp[p] = '\0';

            if (strncmp(tmp, "solucao", 7) == 0 || strncmp(tmp, "solução", 7) == 0)
                return strdup(buf + start);  // devolver o marcador

            continue;  // ignorar outros comentários
        }

        return strdup(buf + start);
    }
}


static void parse_input(const char *filename, Board *init) {
    FILE *f = fopen(filename, "r");
    if (!f) die("fopen");

    char *lines[MAX_ROWS] = {0};
    int r = 0, maxc = 0;

    // Leia tabuleiro inicial
    while (1) {
        char *ln = next_noncomment_line(f);
        if (!ln) break;

        if (ln[0] == '#' && 
            (strncmp(ln, "#solucao", 8) == 0 || strncmp(ln, "#solução", 8) == 0)) {
            free(ln);
            break;
        }

        lines[r] = ln;
        int L = strlen(ln);
        if (L > maxc) maxc = L;
        r++;
        if (r >= MAX_ROWS) break;
    }

    if (r == 0) die("arquivo sem tabuleiro inicial");

    init->rows = r;
    init->cols = maxc;

    for (int i = 0; i < r; i++)
        for (int j = 0; j < maxc; j++)
            init->board[i][j] = (j < strlen(lines[i]) ? lines[i][j] : '.');

    for (int i = 0; i < r; i++)
        free(lines[i]);

    // Ler matriz solução
    char *goal_lines[MAX_ROWS] = {0};
    int gr = 0, gmaxc = 0;

    while (1) {
        char *ln = next_noncomment_line(f);
        if (!ln) break;

        if (ln[0] == '#') {
            free(ln);
            continue;
        }

        goal_lines[gr] = ln;
        int L = strlen(ln);
        if (L > gmaxc) gmaxc = L;
        gr++;

        if (gr >= MAX_ROWS) break;
    }

    if (gr == 0) die("arquivo sem solucao");

    goal_rows = gr;
    goal_cols = gmaxc;

    for (int i = 0; i < gr; i++)
        for (int j = 0; j < gmaxc; j++)
            goal_board[i][j] = (j < strlen(goal_lines[i]) ? goal_lines[i][j] : '0');

    for (int i = 0; i < gr; i++)
        free(goal_lines[i]);

    fclose(f);
}


// ------------------- Detecta peças e suas bounding boxes -------------------
static void detect_pieces(const Board *b) {
    piece_count = 0;
    bool seen[MAX_ROWS][MAX_COLS] = {0};
    for (int i = 0; i < b->rows; ++i) for (int j = 0; j < b->cols; ++j) seen[i][j] = false;

    for (int i = 0; i < b->rows; ++i) {
        for (int j = 0; j < b->cols; ++j) {
            char ch = b->board[i][j];
            if (ch == '.' || ch == '0' || ch == '\0') continue;
            if (seen[i][j]) continue;
            // flood-fill para marcar peça
            char id = ch;
            int minr = i, minc = j, maxr = i, maxc = j;
            // stack simples
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
            // registrar peça
            PieceInfo p; p.id = id; p.minr = minr; p.minc = minc; p.maxr = maxr; p.maxc = maxc; p.h = maxr - minr + 1; p.w = maxc - minc + 1;
            pieces[piece_count++] = p;
            if (isupper((unsigned char)id)) goal_target_id = id; // detecta alvo
            if (piece_count >= MAX_PIECES) return;
        }
    }
}

// ------------------- Heurística -------------------
// Heurística: distância mínima (Manhattan) do centro do bloco-alvo até o centro da área objetivo definida
static int heuristic_board(const Board *b) {
    if (goal_target_id == '\0') return 0;
    // encontra peça alvo
    int tr=-1, tc=-1, th=0, tw=0;
    for (int i = 0; i < piece_count; ++i) if (pieces[i].id == goal_target_id) { tr = pieces[i].minr; tc = pieces[i].minc; th = pieces[i].h; tw = pieces[i].w; break; }
    if (tr == -1) return 0;
    // encontra células na goal_board que são a letra maiúscula (posição alvo)
    int best = INT_MAX;
    for (int i = 0; i <= b->rows - th; ++i) for (int j = 0; j <= b->cols - tw; ++j) {
        bool ok = true; int cost = 0;
        for (int r = 0; r < th && ok; ++r) for (int c = 0; c < tw; ++c) {
            char gch = (i+r < goal_rows && j+c < goal_cols) ? goal_board[i+r][j+c] : '0';
            if (gch == '0') continue; // don't care
            if (gch != goal_target_id) { ok = false; break; }
        }
        if (!ok) continue;
        // custo: Manhattan entre current min corner and this candidate min corner
        cost = abs(tr - i) + abs(tc - j);
        if (cost < best) best = cost;
    }
    if (best == INT_MAX) return 0;
    return best;
}

// ------------------- Geração de movimentos -------------------
// Tenta mover a peça p por (dr,dc). Retorna true se movimento válido e escreve novo board
static bool try_move_piece(const Board *b, const PieceInfo *pinfo, int dr, int dc, Board *out) {
    // cria cópia
    *out = *b;
    // verifica se movimento está dentro dos limites
    int new_minr = pinfo->minr + dr, new_minc = pinfo->minc + dc;
    int new_maxr = pinfo->maxr + dr, new_maxc = pinfo->maxc + dc;
    if (new_minr < 0 || new_minc < 0 || new_maxr >= b->rows || new_maxc >= b->cols) return false;
    // limpar região antiga (colocar '.')
    for (int r = pinfo->minr; r <= pinfo->maxr; ++r) for (int c = pinfo->minc; c <= pinfo->maxc; ++c) {
        if (b->board[r][c] == pinfo->id) out->board[r][c] = '.';
    }
    // verificar colisões na nova posição: cada célula que receberia a peça deve estar '.' or same id (should be '.')
    for (int r = 0; r < pinfo->h; ++r) for (int c = 0; c < pinfo->w; ++c) {
        char cur = b->board[pinfo->minr + r][pinfo->minc + c];
        // determine which cells piece occupies relative to bounding box -- only place cells that equal id in original
        if (cur != pinfo->id) continue; // for pieces with holes or non-rectangular shapes, we move only occupied cells
        int tr = pinfo->minr + r + dr;
        int tc = pinfo->minc + c + dc;
        char dest = b->board[tr][tc];
        if (dest != '.' && dest != pinfo->id) return false;
    }
    // apply piece to new cells
    for (int r = 0; r < pinfo->h; ++r) for (int c = 0; c < pinfo->w; ++c) {
        char cur = b->board[pinfo->minr + r][pinfo->minc + c];
        if (cur != pinfo->id) continue;
        int tr = pinfo->minr + r + dr;
        int tc = pinfo->minc + c + dc;
        out->board[tr][tc] = pinfo->id;
    }
    return true;
}

// Re-detecta peças em novo board para atualizar bounding boxes (simples, chamado ao criar filhos)
static void detect_pieces_in_board(Board *b, PieceInfo out_pieces[], int *out_count) {
    bool seen[MAX_ROWS][MAX_COLS] = {0};
    int cnt = 0;
    for (int i = 0; i < b->rows; ++i) for (int j = 0; j < b->cols; ++j) seen[i][j] = false;
    for (int i = 0; i < b->rows; ++i) for (int j = 0; j < b->cols; ++j) {
        char ch = b->board[i][j];
        if (ch == '.' || ch == '0') continue;
        if (seen[i][j]) continue;
        // flood
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

// ------------------- Goal check -------------------
static bool is_goal_state(const Board *b) {
    // verifica se para todas as células da goal_board que contêm letra maiúscula a mesma letra está naquela posição
    for (int i = 0; i < goal_rows; ++i) for (int j = 0; j < goal_cols; ++j) {
        char gch = goal_board[i][j];
        if (gch == '0') continue;
        if (i >= b->rows || j >= b->cols) return false;
        if (b->board[i][j] != gch) return false;
    }
    return true;
}

// ------------------- A* -------------------
static void reconstruct_and_print_solution(Node *goal_node) {
    // junta movimentos da raiz até goal
    // contar comprimento
    int steps = 0; Node *cur = goal_node;
    while (cur && cur->parent) { steps++; cur = cur->parent; }
    char *moves = malloc(steps * 3 + 1);
    moves[0] = '\0';
    cur = goal_node;
    // iremos montar em ordem reversa
    for (int i = steps-1; i >= 0; --i) {
        // move format: <piece><dir> onde dir: C(up)=U, B(down)=D, E(left)=L, D(right)=R? O enunciado quer letras? Vamos imprimir P<id><dir>
        Node *n = cur;
        char part[8];
        // use diretivas do enunciado: U,D,L,R -> mapeamento para saída: C,B,E,D (como no código original)??
        char outdir = n->move_dir;
        snprintf(part, sizeof(part), "%c%c ", n->move_piece, outdir);
        strcat(moves, part);
        cur = cur->parent;
    }
    // moves está em ordem raiz->goal mas pode conter trailing espaço
    printf("Solução (%d movimentos):\n%s\n", steps, moves);
    free(moves);
}

static void a_star_solve(Board *init) {
    // detect initial pieces
    detect_pieces(init);
    int h0 = heuristic_board(init);
    char *init_str = board_to_str(init);
    // estruturas
    MinHeap *open = heap_create(1024);
    HashEntry **visited = hash_table_create();

    Node *start = malloc(sizeof(Node)); if (!start) die("malloc");
    start->state_str = init_str; start->g = 0; start->f = h0; start->parent = NULL; start->move_piece='\0'; start->move_dir='\0';
    heap_push(open, start);
    hash_put(visited, start->state_str, 0);

    int explored = 0;
    while (open->size > 0) {
        Node *cur = heap_pop(open);
        explored++;
        // converter para board e verificar goal
        Board b; b.rows = init->rows; b.cols = init->cols; str_to_board(cur->state_str, &b);
        if (is_goal_state(&b)) {
            printf("Explorados: %d\n", explored);
            reconstruct_and_print_solution(cur);
            // cleanup: liberar nós remanescentes
            // Observação: por simplicidade não liberamos toda a memória alocada para todos os nodes (poderia ser feito). 
            // Porém alocamos apenas o necessário para resolver o problema.
            heap_free(open); hash_free(visited);
            return;
        }
        // gerar vizinhos
        // detecta peças no board atual
        PieceInfo cur_pieces[MAX_PIECES]; int cur_pc = 0; detect_pieces_in_board(&b, cur_pieces, &cur_pc);
        for (int pi = 0; pi < cur_pc; ++pi) {
            PieceInfo *p = &cur_pieces[pi];
            const int drs[4] = {-1,1,0,0}; const int dcs[4] = {0,0,-1,1};
            const char dirs[4] = {'U','D','L','R'};
            for (int mv = 0; mv < 4; ++mv) {
                Board nb;
                if (!try_move_piece(&b, p, drs[mv], dcs[mv], &nb)) continue;
                // montar string
                char *ns = board_to_str(&nb);
                int prev_g = hash_get_best(visited, ns);
                int ng = cur->g + 1;
                if (ng < prev_g) {
                    int h = 0; // heurística precisa de detection global; recompute pieces for nb to calculate heuristic
                    // detect pieces in nb locally
                    PieceInfo tmp_pieces[MAX_PIECES]; int tmp_pc = 0; detect_pieces_in_board(&nb, tmp_pieces, &tmp_pc);
                    // temporariamente substitui global pieces para heuristic function
                    // copy
                    int saved_pc = piece_count;
                    memcpy(pieces, tmp_pieces, sizeof(PieceInfo)*tmp_pc);
                    piece_count = tmp_pc;
                    h = heuristic_board(&nb);
                    // restore piece_count (we don't need previous content for next iter because it'll be recomputed from state)
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
        // NOTE: Não liberamos cur->state_str e cur ao expandir para manter parent chain (poderíamos gerenciar com lista para free)
    }

    printf("Solução não encontrada (A* esgotou).\n");
    heap_free(open); hash_free(visited);
}

// ------------------- main -------------------
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
