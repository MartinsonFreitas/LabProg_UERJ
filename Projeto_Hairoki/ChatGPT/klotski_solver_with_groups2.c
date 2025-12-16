/* klotski_solver_with_groups.c
 *
 * Versão com suporte a grupos (opção B).
 * - canonicaliza estados por grupo para reduzir simetrias.
 * - is_goal_state aceita qualquer membro do grupo.
 *
 * Compilar: gcc -O2 -std=c11 -o klotski_solver_with_groups klotski_solver_with_groups.c
 * Ex.: ./klotski_solver_with_groups 18passos.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_ROWS 20
#define MAX_COLS 40
#define MAX_CELLS (MAX_ROWS * MAX_COLS)
#define MAX_PIECES 52
#define HASH_SIZE 300001
#define MAX_GROUPS 128
#define MAX_GROUP_MEMBERS 128

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

static void str_to_board(const char *s, Board *b);

/* Globals */
static PieceInfo pieces[MAX_PIECES];
static int piece_count = 0;
static char goal_board[MAX_ROWS][MAX_COLS];
static int goal_rows = 0, goal_cols = 0;
static char goal_target_id = '\0';

/* Group structures */
static int group_of[256]; /* map char -> group id or -1 */
static int groups_count = 0;
static unsigned char group_members[MAX_GROUPS][MAX_GROUP_MEMBERS];
static int group_sizes[MAX_GROUPS];

/* Utilities */
static void die(const char *msg) { perror(msg); exit(EXIT_FAILURE); }
static char *xstrdup(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (!p) die("malloc");
    memcpy(p, s, n);
    return p;
}

/* Board <-> string (canonicalized) */
static char *board_to_str(const Board *b) {
    int n = b->rows * b->cols;
    char *s = malloc(n + 1);
    if (!s) die("malloc");
    int k = 0;
    for (int i = 0; i < b->rows; ++i) for (int j = 0; j < b->cols; ++j) s[k++] = b->board[i][j];
    s[n] = '\0';
    return s;
}

/* Produce a canonical string applying group canonicalization:
   for each group, collect positions containing any member, sort positions,
   sort member chars, then place sorted chars into sorted positions.
   This returns a heap-allocated string suitable for hashing/comparison. */
static char *board_to_canonical_str(const Board *b) {
    /* copy board into temporary linear array */
    int R = b->rows, C = b->cols;
    int N = R * C;
    char *tmp = malloc(N);
    if (!tmp) die("malloc");
    for (int i = 0, k = 0; i < R; ++i) for (int j = 0; j < C; ++j) tmp[k++] = b->board[i][j];

    /* For each group, collect positions and member chars present */
    for (int g = 0; g < groups_count; ++g) {
        int pos_count = 0;
        int maxpos = N;
        int *positions = malloc(sizeof(int) * maxpos);
        unsigned char *chars = malloc(sizeof(unsigned char) * group_sizes[g]);
        if (!positions || !chars) die("malloc");

        /* collect positions and corresponding chars */
        for (int idx = 0; idx < N; ++idx) {
            unsigned char ch = (unsigned char)tmp[idx];
            if (ch == '.' || ch == '0') continue;
            if (group_of[ch] == g) {
                positions[pos_count] = idx;
                chars[pos_count] = ch;
                pos_count++;
            }
        }

        if (pos_count > 1) {
            /* sort positions ascending */
            for (int a = 0; a < pos_count-1; ++a)
                for (int bpos = a+1; bpos < pos_count; ++bpos)
                    if (positions[a] > positions[bpos]) {
                        int t = positions[a]; positions[a] = positions[bpos]; positions[bpos] = t;
                    }

            /* sort chars ascending */
            for (int a = 0; a < pos_count-1; ++a)
                for (int bch = a+1; bch < pos_count; ++bch)
                    if (chars[a] > chars[bch]) {
                        unsigned char t = chars[a]; chars[a] = chars[bch]; chars[bch] = t;
                    }

            /* write sorted chars into sorted positions */
            for (int p = 0; p < pos_count; ++p) tmp[positions[p]] = chars[p];
        }
        free(positions);
        free(chars);
    }

    /* allocate final string */
    char *s = malloc(N + 1);
    if (!s) die("malloc");
    memcpy(s, tmp, N);
    s[N] = '\0';
    free(tmp);
    return s;
}

/* Print board */
static void print_board(const Board *b) {
    for (int i = 0; i < b->rows; ++i) {
        for (int j = 0; j < b->cols; ++j) putchar(b->board[i][j]);
        putchar('\n');
    }
}

/* Heap (min by f) */
static MinHeap *heap_create(int cap) {
    MinHeap *h = malloc(sizeof(MinHeap)); if (!h) die("malloc");
    h->data = malloc(sizeof(Node*) * cap); if (!h->data) die("malloc");
    h->size = 0; h->cap = cap; return h;
}
static void heap_swap(Node **a, Node **b) { Node *t = *a; *a = *b; *b = t; }
static void heap_push(MinHeap *h, Node *node) {
    if (h->size >= h->cap) { h->cap *= 2; h->data = realloc(h->data, sizeof(Node*) * h->cap); if (!h->data) die("realloc"); }
    int i = h->size++; h->data[i] = node;
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
        heap_swap(&h->data[i], &h->data[smallest]); i = smallest;
    }
    return ret;
}
static void heap_free(MinHeap *h) { free(h->data); free(h); }

/* Hash table */
static HashEntry **hash_table_create(void) {
    HashEntry **t = calloc(HASH_SIZE, sizeof(HashEntry*)); if (!t) die("calloc"); return t;
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
    e->state_str = xstrdup(s); e->g = g; e->next = t[idx]; t[idx] = e;
}
static void hash_free(HashEntry **t) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashEntry *e = t[i];
        while (e) { HashEntry *n = e->next; free(e->state_str); free(e); e = n; }
    }
    free(t);
}

/* ---------------- Parsing (inclui leitura de grupos) ---------------- */

/* next_trimmed_line: retorna próxima linha sem espaços nas bordas, NULL no EOF */
static char *next_trimmed_line(FILE *f) {
    static char buf[4096];
    while (1) {
        if (!fgets(buf, sizeof(buf), f)) return NULL;
        size_t len = strlen(buf);
        while (len > 0 && (buf[len-1] == '\n' || buf[len-1] == '\r')) buf[--len] = '\0';
        size_t start = 0;
        while (start < len && isspace((unsigned char)buf[start])) start++;
        size_t end = len;
        while (end > start && isspace((unsigned char)buf[end-1])) end--;
        if (end <= start) continue;
        char *out = malloc(end - start + 1);
        if (!out) die("malloc");
        memcpy(out, buf + start, end - start); out[end - start] = '\0';
        return out;
    }
}

/* parse groups line like "a b c d" into group_members[gid] */
static void parse_group_line(int gid, const char *line) {
    int pos = 0;
    const char *p = line;
    while (*p) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;
        char ch = *p;
        /* accept single-character tokens */
        if (!isprint((unsigned char)ch)) break;
        group_members[gid][pos++] = (unsigned char)ch;
        group_of[(unsigned char)ch] = gid;
        p++;
        /* skip rest of token until space (handles accidental multi-char tokens) */
        while (*p && !isspace((unsigned char)*p)) p++;
    }
    group_sizes[gid] = pos;
}

/* parse_input: initial board, #solucao, goal board, optional #grupos
   groups format: a line with number G, then G lines each with members separated by spaces */
static void parse_input(const char *filename, Board *init) {
    for (int i = 0; i < 256; ++i) group_of[i] = -1;
    groups_count = 0;
    for (int g = 0; g < MAX_GROUPS; ++g) group_sizes[g] = 0;

    FILE *f = fopen(filename, "r"); if (!f) die("fopen");
    char *lines[MAX_ROWS];
    int r = 0, maxc = 0;

    /* find first non-comment line for initial board */
    char *ln = NULL;
    while (1) {
        ln = next_trimmed_line(f);
        if (!ln) die("Arquivo terminou antes do tabuleiro inicial");
        if (ln[0] == '#') { free(ln); continue; }
        break;
    }
    if (r < MAX_ROWS) { lines[r++] = ln; int L = strlen(ln); if (L > maxc) maxc = L; } else { free(ln); die("Tabuleiro inicial grande demais"); }

    /* read rest of initial board until #solucao */
    while (1) {
        ln = next_trimmed_line(f);
        if (!ln) die("Arquivo terminou antes de #solucao");
        if (ln[0] == '#') {
            if (strncmp(ln, "#solucao", 8) == 0 || strncmp(ln, "#solução", 8) == 0) { free(ln); break; }
            free(ln); continue;
        }
        if (r >= MAX_ROWS) { free(ln); die("Tabuleiro inicial grande demais"); }
        lines[r++] = ln; int L = strlen(ln); if (L > maxc) maxc = L;
    }

    init->rows = r; init->cols = maxc;
    for (int i = 0; i < r; ++i) {
        int L = strlen(lines[i]);
        for (int j = 0; j < maxc; ++j) init->board[i][j] = (j < L ? lines[i][j] : '.');
        free(lines[i]);
    }
    
//
        /* ----------- LEITURA DO TABULEIRO SOLUÇÃO (ACEITA #grupos OU linha com número) --------- */
    char *goal_lines[MAX_ROWS];
    int gr = 0, gmaxc = 0;

    /* Leitura: paramos se:
       - encontramos uma linha que começa com '#' e é "#grupos" (ou "#grupos" com acento),
       - ou se encontramos uma linha que é apenas um número (contagem de grupos) -> voltaremos a processá-la.
       - ou EOF. */
    char *maybe_group_count_line = NULL;

    while (1) {
        char *ln = next_trimmed_line(f);
        if (!ln) break; /* EOF */

        if (ln[0] == '#') {
            /* marcador de grupos? */
            if (strncmp(ln, "#grupos", 7) == 0 || strncmp(ln, "#grupos", 7) == 0) {
                free(ln);
                break;
            }
            /* outras linhas de comentário: ignorar */
            free(ln);
            continue;
        }

        /* caso especial: linha com apenas dígitos (ex: "4") -> significa que o arquivo
           não possui "#grupos" e esta linha é a contagem de grupos. Guardamos e saímos. */
        bool all_digits = true;
        for (size_t p = 0; ln[p] != '\0'; ++p) if (!isdigit((unsigned char)ln[p])) { all_digits = false; break; }
        if (all_digits) {
            maybe_group_count_line = ln; /* não free aqui: usaremos esta linha ao parsear grupos */
            break;
        }

        /* caso normal: linha do tabuleiro objetivo */
        if (gr >= MAX_ROWS) { free(ln); die("Tabuleiro solução grande demais"); }
        goal_lines[gr++] = ln;
        int L = strlen(ln);
        if (L > gmaxc) gmaxc = L;
    }

    if (gr == 0) die("arquivo sem solucao");
    goal_rows = gr;
    goal_cols = gmaxc;
    for (int i = 0; i < gr; i++) {
        int L = strlen(goal_lines[i]);
        for (int j = 0; j < gmaxc; j++)
            goal_board[i][j] = (j < L ? goal_lines[i][j] : '0');
        free(goal_lines[i]);
    }

    /* ----------- LEITURA DE GRUPOS (suporta: com marcador; sem marcador; número isolado) --------- */
    /* if maybe_group_count_line != NULL -> it already contains the line with the number G */
    if (maybe_group_count_line) {
        int G = atoi(maybe_group_count_line);
        free(maybe_group_count_line);
        for (int g = 0; g < G; ++g) {
            char *ln = next_trimmed_line(f);
            if (!ln) break;
            parse_group_line(groups_count, ln);
            free(ln);
            groups_count++;
            if (groups_count >= MAX_GROUPS) break;
        }
    } else {
        /* tenta ler próxima linha; se for número -> parse groups; senão: put back (não necessário aqui) */
        char *ln = next_trimmed_line(f);
        if (ln) {
            if (isdigit((unsigned char)ln[0])) {
                int G = atoi(ln); free(ln);
                for (int g = 0; g < G; ++g) {
                    char *ln2 = next_trimmed_line(f);
                    if (!ln2) break;
                    parse_group_line(groups_count, ln2);
                    free(ln2);
                    groups_count++;
                    if (groups_count >= MAX_GROUPS) break;
                }
            } else {
                /* se não é número nem comentário, apenas ignoramos (formato inesperado) */
                free(ln);
            }
        }
    }

}
/* ---------------- piece detection and moves ---------------- */

/* Detect pieces by character (each character occurrence is part of its piece) */
static void detect_pieces(const Board *b) {
    piece_count = 0;
    bool seen[MAX_ROWS][MAX_COLS] = {0};
    for (int i = 0; i < b->rows; ++i) for (int j = 0; j < b->cols; ++j) seen[i][j] = false;
    for (int i = 0; i < b->rows; ++i) {
        for (int j = 0; j < b->cols; ++j) {
            char ch = b->board[i][j];
            if (ch == '.' || ch == '0' || ch == '\0') continue;
            if (seen[i][j]) continue;
            /* For this code we treat contiguous same-letter regions as one piece (preserves X 2x2)
               BUT for unit letters lying alone, it's a 1x1 piece. */
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
            PieceInfo p; p.id = id; p.minr = minr; p.minc = minc; p.maxr = maxr; p.maxc = maxc;
            p.h = maxr - minr + 1; p.w = maxc - minc + 1;
            pieces[piece_count++] = p;
            if (isupper((unsigned char)id)) goal_target_id = id;
            if (piece_count >= MAX_PIECES) return;
        }
    }
}

/* Detect pieces in given board (for children generation) */
static void detect_pieces_in_board(Board *b, PieceInfo out_pieces[], int *out_count) {
    bool seen[MAX_ROWS][MAX_COLS] = {0};
    int cnt = 0;
    for (int i = 0; i < b->rows; ++i) for (int j = 0; j < b->cols; ++j) seen[i][j] = false;
    for (int i = 0; i < b->rows; ++i) for (int j = 0; j < b->cols; ++j) {
        char ch = b->board[i][j];
        if (ch == '.' || ch == '0') continue;
        if (seen[i][j]) continue;
        /* flood */
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
        PieceInfo p; p.id = ch; p.minr = minr; p.minc = minc; p.maxr = maxr; p.maxc = maxc;
        p.h = maxr - minr + 1; p.w = maxc - minc + 1;
        out_pieces[cnt++] = p; if (cnt >= MAX_PIECES) break;
    }
    *out_count = cnt;
}

/* Heuristic: Manhattan of top-left corner of target block to mask positions */
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
            /* accept if goal letter equals target id or is in same group as target id */
            if (gch != goal_target_id) { ok = false; break; }
        }
        if (!ok) continue;
        cost = abs(tr - i) + abs(tc - j);
        if (cost < best) best = cost;
    }
    if (best == INT_MAX) return 0;
    return best;
}

/* try_move_piece: move only cells actually belonging to piece (not whole bbox) */
static bool try_move_piece(const Board *b, const PieceInfo *pinfo, int dr, int dc, Board *out) {
    *out = *b;
    int new_minr = pinfo->minr + dr, new_minc = pinfo->minc + dc;
    int new_maxr = pinfo->maxr + dr, new_maxc = pinfo->maxc + dc;
    if (new_minr < 0 || new_minc < 0 || new_maxr >= b->rows || new_maxc >= b->cols) return false;

    /* Collect all cells that are part of this piece (cells equal to pinfo->id within its bbox) */
    int cells_r[MAX_CELLS], cells_c[MAX_CELLS], ncells = 0;
    for (int r = pinfo->minr; r <= pinfo->maxr; ++r) for (int c = pinfo->minc; c <= pinfo->maxc; ++c) {
        if (b->board[r][c] == pinfo->id) { cells_r[ncells] = r; cells_c[ncells] = c; ncells++; }
    }

    /* clear original cells */
    for (int k = 0; k < ncells; ++k) out->board[cells_r[k]][cells_c[k]] = '.';

    /* check collisions on destination cells */
    for (int k = 0; k < ncells; ++k) {
        int tr = cells_r[k] + dr, tc = cells_c[k] + dc;
        char dest = b->board[tr][tc];
        if (dest != '.' && dest != pinfo->id) {
            /* collision -> restore original (we used 'out' copy, but we can just return false) */
            return false;
        }
    }

    /* apply piece to new cells */
    for (int k = 0; k < ncells; ++k) {
        int tr = cells_r[k] + dr, tc = cells_c[k] + dc;
        out->board[tr][tc] = pinfo->id;
    }
    return true;
}

/* is_goal_state: compare only non-'0' cells; accept if equal OR same group */
static bool is_goal_state(const Board *b) {
    for (int i = 0; i < goal_rows; ++i) for (int j = 0; j < goal_cols; ++j) {
        char gch = goal_board[i][j];
        if (gch == '0') continue;
        if (i >= b->rows || j >= b->cols) return false;
        char sch = b->board[i][j];
        if (sch == gch) continue;
        int gg = group_of[(unsigned char)gch];
        int sg = group_of[(unsigned char)sch];
        if (gg != -1 && gg == sg) continue; /* same group accepted */
        return false;
    }
    return true;
}

/* reconstruct solution print */
static void reconstruct_and_print_solution(Node *goal_node) {
    int steps = 0;
    for (Node *n = goal_node; n && n->parent; n = n->parent) steps++;
    if (steps == 0) { printf("Solução: 0 movimentos (estado inicial é objetivo)\n"); return; }
    Node **arr = malloc(sizeof(Node*) * steps); if (!arr) die("malloc");
    Node *cur = goal_node;
    for (int i = steps - 1; i >= 0; --i) { arr[i] = cur; cur = cur->parent; }
    printf("Solução (%d movimentos):\n", steps);
    for (int i = 0; i < steps; ++i) {
        char piece = arr[i]->move_piece;
        char dir = arr[i]->move_dir;
        printf("%c%c", piece ? piece : '?', dir ? dir : '?');
        if (i+1 < steps) putchar(' ');
    }
    putchar('\n');
    free(arr);
}

/* A* search (uses canonical state strings for visited) */
static void a_star_solve(Board *init) {
    detect_pieces(init);
    int h0 = heuristic_board(init);
    char *init_str = board_to_canonical_str(init);

    MinHeap *open = heap_create(2048);
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
        /* NOTE: cur->state_str is canonical; str_to_board writes chars in canonical order,
           but that's fine: we only need a valid board for move generation. */
        if (is_goal_state(&b)) {
            printf("Explorados: %d\n", explored);
            reconstruct_and_print_solution(cur);
            heap_free(open); hash_free(visited);
            return;
        }

        PieceInfo cur_pieces[MAX_PIECES]; int cur_pc = 0; detect_pieces_in_board(&b, cur_pieces, &cur_pc);
        for (int pi = 0; pi < cur_pc; ++pi) {
            PieceInfo *p = &cur_pieces[pi];
            const int drs[4] = {-1,1,0,0}; const int dcs[4] = {0,0,-1,1};
            const char dirs[4] = {'U','D','L','R'};
            for (int mv = 0; mv < 4; ++mv) {
                Board nb;
                if (!try_move_piece(&b, p, drs[mv], dcs[mv], &nb)) continue;
                char *ns = board_to_canonical_str(&nb);
                int prev_g = hash_get_best(visited, ns);
                int ng = cur->g + 1;
                if (ng < prev_g) {
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
        /* not freeing cur or cur->state_str to preserve parent chain */
    }

    printf("Solução não encontrada (A* esgotou).\n");
    heap_free(open); hash_free(visited);
}

/* helper to write canonical string into board (for move generation) */
static void str_to_board(const char *s, Board *b) {
    int k = 0;
    for (int i = 0; i < b->rows; ++i) for (int j = 0; j < b->cols; ++j) b->board[i][j] = s[k++];
}

/* main */
int main(int argc, char *argv[]) {
    if (argc != 2) { fprintf(stderr, "Uso: %s <arquivo_puzzle.txt>\n", argv[0]); return EXIT_FAILURE; }
    Board init;
    parse_input(argv[1], &init);
    printf("Tabuleiro inicial (%dx%d):\n", init.rows, init.cols); print_board(&init);
    printf("Tabuleiro objetivo (%dx%d):\n", goal_rows, goal_cols);
    for (int i = 0; i < goal_rows; ++i) { for (int j = 0; j < goal_cols; ++j) putchar(goal_board[i][j]); putchar('\n'); }
    a_star_solve(&init);
    return 0;
}
