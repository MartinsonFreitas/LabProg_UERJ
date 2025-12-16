/*
 * klotski_solver_final_v10.c
 *
 * Versão final com parser universal e suporte a grupos (opção B).
 *
 * Compile:
 *   gcc -O2 -std=c11 -o klotski_solver_final_v10 klotski_solver_final_v10.c
 *
 * Run:
 *   ./klotski_solver_final_v10 18passos.txt
 *   ./klotski_solver_final_v10 engarrafamento.txt
 *
 * Notes:
 * - Parser aceita formatos com "#grupos" ou sem (linha com número G).
 * - '0' no objetivo é wildcard (don't-care).
 * - Groups canonicalization reduces symmetric states.
 * - Pieces: same-letter connected cells = one piece (keeps X 2x2).
 * - Movement format: Peça+Direção (U/D/L/R).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>

/* Limits */
#define MAX_ROWS 40
#define MAX_COLS 80
#define MAX_CELLS (MAX_ROWS * MAX_COLS)
#define MAX_PIECES 256
#define HASH_SIZE 500009
#define MAX_GROUPS 128
#define MAX_GROUP_MEMBERS 128

/* Types */
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
    char *state_str;          /* canonical state string */
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

/* Prototypes */
static void str_to_board(const char *s, Board *b);

/* Globals */
static PieceInfo pieces[MAX_PIECES];
static int piece_count = 0;
static char goal_board[MAX_ROWS][MAX_COLS];
static int goal_rows = 0, goal_cols = 0;
static char goal_target_id = '\0';

/* Groups */
static int group_of[256];
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

/* Board <-> string (non-canonical) */
static char *board_to_str(const Board *b) {
    int n = b->rows * b->cols;
    char *s = malloc(n + 1);
    if (!s) die("malloc");
    int k = 0;
    for (int i = 0; i < b->rows; ++i)
        for (int j = 0; j < b->cols; ++j)
            s[k++] = b->board[i][j];
    s[n] = '\0';
    return s;
}

/* Canonical string for hashing: apply group canonicalization */
static char *board_to_canonical_str(const Board *b) {
    int R = b->rows, C = b->cols;
    int N = R * C;
    char *tmp = malloc(N);
    if (!tmp) die("malloc");

    /* linear copy */
    int k = 0;
    for (int i = 0; i < R; ++i)
        for (int j = 0; j < C; ++j)
            tmp[k++] = b->board[i][j];

    /* for each group, gather positions and characters then sort */
    for (int g = 0; g < groups_count; ++g) {
        int pos_count = 0;
        int *positions = malloc(sizeof(int) * N);
        unsigned char *chars = malloc(sizeof(unsigned char) * N);
        if (!positions || !chars) die("malloc");

        for (int i = 0; i < N; ++i) {
            unsigned char ch = (unsigned char)tmp[i];
            if (ch == '.' || ch == '0') continue;
            if (group_of[ch] == g) {
                positions[pos_count] = i;
                chars[pos_count] = ch;
                pos_count++;
            }
        }

        if (pos_count > 1) {
            /* simple sorts (pos_count small) */
            for (int a = 0; a < pos_count - 1; ++a)
                for (int b2 = a + 1; b2 < pos_count; ++b2)
                    if (positions[a] > positions[b2]) {
                        int t = positions[a]; positions[a] = positions[b2]; positions[b2] = t;
                    }

            for (int a = 0; a < pos_count - 1; ++a)
                for (int b2 = a + 1; b2 < pos_count; ++b2)
                    if (chars[a] > chars[b2]) {
                        unsigned char t = chars[a]; chars[a] = chars[b2]; chars[b2] = t;
                    }

            for (int p = 0; p < pos_count; ++p) tmp[positions[p]] = chars[p];
        }
        free(positions); free(chars);
    }

    char *out = malloc(N + 1);
    if (!out) die("malloc");
    memcpy(out, tmp, N);
    out[N] = '\0';
    free(tmp);
    return out;
}

/* print board */
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
    h->size = 0; h->cap = cap;
    return h;
}
static void heap_swap(Node **a, Node **b) { Node *t = *a; *a = *b; *b = t; }
static void heap_push(MinHeap *h, Node *node) {
    if (h->size >= h->cap) {
        h->cap *= 2;
        h->data = realloc(h->data, sizeof(Node*) * h->cap);
        if (!h->data) die("realloc");
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

/* Hash table visited */
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
    e->state_str = xstrdup(s); e->g = g; e->next = t[idx]; t[idx] = e;
}
static void hash_free(HashEntry **t) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashEntry *e = t[i];
        while (e) { HashEntry *n = e->next; free(e->state_str); free(e); e = n; }
    }
    free(t);
}

/* ---------------- Parsing (universal) ---------------- */

/* read a line, trim edges, return malloc'd string or NULL on EOF */
static char *next_trimmed_line(FILE *f) {
    static char buf[8192];
    while (1) {
        if (!fgets(buf, sizeof(buf), f)) return NULL;
        size_t len = strlen(buf);
        while (len > 0 && (buf[len-1] == '\n' || buf[len-1] == '\r')) buf[--len] = '\0';
        size_t s = 0;
        while (s < len && isspace((unsigned char)buf[s])) s++;
        size_t e = len;
        while (e > s && isspace((unsigned char)buf[e-1])) e--;
        if (e <= s) continue; /* empty line */
        size_t L = e - s;
        char *out = malloc(L + 1);
        if (!out) die("malloc");
        memcpy(out, buf + s, L);
        out[L] = '\0';
        return out;
    }
}

/* parse a group line like "a b c d" */
static void parse_group_line(int gid, const char *line) {
    int pos = 0;
    const char *p = line;
    while (*p) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;
        unsigned char ch = (unsigned char)*p;
        group_members[gid][pos++] = ch;
        group_of[ch] = gid;
        p++;
        while (*p && !isspace((unsigned char)*p)) p++;
    }
    group_sizes[gid] = pos;
}

/* universal parser: supports formats with or without "#grupos" */
static void parse_input(const char *filename, Board *init) {
    /* init groups */
    for (int i = 0; i < 256; ++i) group_of[i] = -1;
    groups_count = 0;
    for (int i = 0; i < MAX_GROUPS; ++i) group_sizes[i] = 0;

    FILE *f = fopen(filename, "r");
    if (!f) die("fopen");

    char *lines_init[MAX_ROWS];
    int ri = 0;
    int maxci = 0;

    /* read initial board until "#solucao" */
    while (1) {
        char *ln = next_trimmed_line(f);
        if (!ln) die("arquivo sem #solucao");
        if (ln[0] == '#') {
            if (strncmp(ln, "#solucao", 8) == 0 || strncmp(ln, "#solução", 8) == 0) { free(ln); break; }
            free(ln);
            continue;
        }
        if (ri >= MAX_ROWS) { free(ln); die("Tabuleiro inicial grande demais"); }
        lines_init[ri++] = ln;
        int L = strlen(ln); if (L > maxci) maxci = L;
    }

    init->rows = ri; init->cols = maxci;
    for (int i = 0; i < ri; ++i) {
        int L = strlen(lines_init[i]);
        for (int j = 0; j < maxci; ++j) init->board[i][j] = (j < L ? lines_init[i][j] : '.');
        free(lines_init[i]);
    }

    /* read goal until '#grupos' OR a digits-only line (G) */
    char *goal_lines[MAX_ROWS];
    int gr = 0, gmaxc = 0;
    char *saved_group_line = NULL;

    while (1) {
        char *ln = next_trimmed_line(f);
        if (!ln) break;
        if (ln[0] == '#') {
            if (strncmp(ln, "#grupos", 7) == 0) { free(ln); break; }
            free(ln); continue;
        }
        /* check digits-only (group count) */
        bool all_digits = true;
        for (size_t p = 0; ln[p]; ++p) if (!isdigit((unsigned char)ln[p])) { all_digits = false; break; }
        if (all_digits) {
            saved_group_line = ln;
            break;
        }
        if (gr >= MAX_ROWS) { free(ln); die("Tabuleiro solucao grande demais"); }
        goal_lines[gr++] = ln;
        int L = strlen(ln); if (L > gmaxc) gmaxc = L;
    }

    if (gr == 0) { fclose(f); die("arquivo sem solucao"); }
    goal_rows = gr; goal_cols = gmaxc;
    for (int i = 0; i < gr; ++i) {
        int L = strlen(goal_lines[i]);
        for (int j = 0; j < gmaxc; ++j) goal_board[i][j] = (j < L ? goal_lines[i][j] : '0');
        free(goal_lines[i]);
    }

    /* parse groups */
    if (saved_group_line) {
        int G = atoi(saved_group_line); free(saved_group_line);
        for (int g = 0; g < G; ++g) {
            char *ln = next_trimmed_line(f);
            if (!ln) break;
            parse_group_line(groups_count, ln);
            free(ln); groups_count++;
            if (groups_count >= MAX_GROUPS) break;
        }
    } else {
        /* maybe next line is '#grupos' or number G */
        char *ln = next_trimmed_line(f);
        if (!ln) { fclose(f); return; }
        if (ln[0] == '#') {
            free(ln);
            char *ln2 = next_trimmed_line(f);
            if (!ln2) { fclose(f); return; }
            int G = atoi(ln2); free(ln2);
            for (int g = 0; g < G; ++g) {
                char *ln3 = next_trimmed_line(f);
                if (!ln3) break;
                parse_group_line(groups_count, ln3);
                free(ln3); groups_count++;
                if (groups_count >= MAX_GROUPS) break;
            }
        } else {
            /* maybe it's a numeric line */
            bool all_digits = true;
            for (size_t p = 0; ln[p]; ++p) if (!isdigit((unsigned char)ln[p])) { all_digits = false; break; }
            if (all_digits) {
                int G = atoi(ln); free(ln);
                for (int g = 0; g < G; ++g) {
                    char *ln3 = next_trimmed_line(f);
                    if (!ln3) break;
                    parse_group_line(groups_count, ln3);
                    free(ln3); groups_count++;
                    if (groups_count >= MAX_GROUPS) break;
                }
            } else {
                free(ln);
            }
        }
    }

    fclose(f);
}

/* ---------------- piece detection and moves ---------------- */

/* detect pieces by flood-fill (keeps multi-cell X block) */
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
            PieceInfo p;
            p.id = id; p.minr = minr; p.minc = minc; p.maxr = maxr; p.maxc = maxc;
            p.h = maxr - minr + 1; p.w = maxc - minc + 1;
            pieces[piece_count++] = p;
            if (isupper((unsigned char)id)) goal_target_id = id;
            if (piece_count >= MAX_PIECES) return;
        }
    }
}

/* detect pieces in a board for generating neighbors */
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
        PieceInfo p; p.id = ch; p.minr = minr; p.minc = minc; p.maxr = maxr; p.maxc = maxc;
        p.h = maxr - minr + 1; p.w = maxc - minc + 1;
        out_pieces[cnt++] = p; if (cnt >= MAX_PIECES) break;
    }
    *out_count = cnt;
}

/* heuristic: manhattan of top-left of target block to any matching placement in goal mask */
static int heuristic_board(const Board *b) {
    if (goal_target_id == '\0') return 0;
    int tr=-1, tc=-1, th=0, tw=0;
    for (int i = 0; i < piece_count; ++i) if (pieces[i].id == goal_target_id) { tr = pieces[i].minr; tc = pieces[i].minc; th = pieces[i].h; tw = pieces[i].w; break; }
    if (tr == -1) return 0;
    int best = INT_MAX;
    for (int i = 0; i <= b->rows - th; ++i) for (int j = 0; j <= b->cols - tw; ++j) {
        bool ok = true;
        for (int r = 0; r < th && ok; ++r) for (int c = 0; c < tw; ++c) {
            char gch = (i+r < goal_rows && j+c < goal_cols) ? goal_board[i+r][j+c] : '0';
            if (gch == '0') continue;
            if (gch != goal_target_id) { ok = false; break; }
        }
        if (!ok) continue;
        int cost = abs(tr - i) + abs(tc - j);
        if (cost < best) best = cost;
    }
    if (best == INT_MAX) return 0;
    return best;
}

/* try_move_piece: move only the actual occupied cells of the piece (not entire bbox) */
static bool try_move_piece(const Board *b, const PieceInfo *pinfo, int dr, int dc, Board *out) {
    *out = *b;
    int new_minr = pinfo->minr + dr, new_minc = pinfo->minc + dc;
    int new_maxr = pinfo->maxr + dr, new_maxc = pinfo->maxc + dc;
    if (new_minr < 0 || new_minc < 0 || new_maxr >= b->rows || new_maxc >= b->cols) return false;

    /* collect actual cells of piece */
    int cells_r[MAX_CELLS], cells_c[MAX_CELLS], nc = 0;
    for (int r = pinfo->minr; r <= pinfo->maxr; ++r)
        for (int c = pinfo->minc; c <= pinfo->maxc; ++c)
            if (b->board[r][c] == pinfo->id) { cells_r[nc] = r; cells_c[nc] = c; nc++; }

    /* clear them in out */
    for (int k = 0; k < nc; ++k) out->board[cells_r[k]][cells_c[k]] = '.';

    /* check collisions */
    for (int k = 0; k < nc; ++k) {
        int tr = cells_r[k] + dr, tc = cells_c[k] + dc;
        char dest = b->board[tr][tc];
        if (dest != '.' && dest != pinfo->id) {
            return false;
        }
    }

    /* write piece at new positions */
    for (int k = 0; k < nc; ++k) {
        int tr = cells_r[k] + dr, tc = cells_c[k] + dc;
        out->board[tr][tc] = pinfo->id;
    }
    return true;
}

/* is_goal_state: compare only cells where goal_board != '0'; accept same char or same group */
static bool is_goal_state(const Board *b) {
    for (int i = 0; i < goal_rows; ++i) for (int j = 0; j < goal_cols; ++j) {
        char gch = goal_board[i][j];
        if (gch == '0') continue;
        if (i >= b->rows || j >= b->cols) return false;
        char sch = b->board[i][j];
        if (sch == gch) continue;
        int gg = group_of[(unsigned char)gch];
        int sg = group_of[(unsigned char)sch];
        if (gg != -1 && gg == sg) continue;
        return false;
    }
    return true;
}

/* reconstruct solution and print moves from root to goal */
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
        printf("%c%c", piece ? piece : '?', dir ? dir : '?');
        if (i+1 < steps) putchar(' ');
    }
    putchar('\n');
    free(arr);
}

/* A* search using canonical strings for visited table */
static void a_star_solve(Board *init) {
    detect_pieces(init);
    int h0 = heuristic_board(init);
    char *init_str = board_to_canonical_str(init);

    MinHeap *open = heap_create(4096);
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
            heap_free(open);
            hash_free(visited);
            return;
        }

        PieceInfo cur_pieces[MAX_PIECES]; int cur_pc = 0;
        detect_pieces_in_board(&b, cur_pieces, &cur_pc);
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
                    memcpy(pieces, tmp_pieces, sizeof(PieceInfo) * tmp_pc);
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
        /* Note: we intentionally keep nodes allocated to preserve parent chain for reconstruction.
           For long runs you may want to store nodes in a list and free after use. */
    }

    printf("Solução não encontrada (A* esgotou).\n");
    heap_free(open);
    hash_free(visited);
}

/* helper: write linear canonical string into board (for neighbor generation)
   (must be consistent with board_to_canonical_str layout) */
static void str_to_board(const char *s, Board *b) {
    int k = 0;
    for (int i = 0; i < b->rows; ++i)
        for (int j = 0; j < b->cols; ++j)
            b->board[i][j] = s[k++];
}

/* main */
int main(int argc, char *argv[]) {
    if (argc != 2) { fprintf(stderr, "Uso: %s <arquivo_puzzle.txt>\n", argv[0]); return EXIT_FAILURE; }

    Board init;
    parse_input(argv[1], &init);

    printf("Tabuleiro inicial (%dx%d):\n", init.rows, init.cols);
    print_board(&init);
    printf("Tabuleiro objetivo (%dx%d):\n", goal_rows, goal_cols);
    for (int i = 0; i < goal_rows; ++i) { for (int j = 0; j < goal_cols; ++j) putchar(goal_board[i][j]); putchar('\n'); }

    a_star_solve(&init);
    return 0;
}
