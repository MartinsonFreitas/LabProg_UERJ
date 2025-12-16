/* * klotski_opt_v2.c - Solucionador Otimizado e Validado
 * Compilar: gcc -O3 -o klotski klotski_opt_v2.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h> // Para usleep (opcional)

// --- Configurações ---
#define MAX_ROWS 20
#define MAX_COLS 20
#define MAX_PIECES 32
#define HASH_SIZE 1000003 

// --- Estruturas ---

typedef struct {
    char id;
    int r, c;     // Posição (topo-esquerda)
    int h, w;     // Altura e largura
    int group_id; // ID do grupo para canonicalização
} Piece;

typedef struct {
    char grid[MAX_ROWS][MAX_COLS]; // Usado apenas para colisão rápida
    Piece pieces[MAX_PIECES];      // Usado para gerar hash e visualização
    int num_pieces;
    int rows, cols;
} State;

typedef struct Node {
    State state;
    struct Node *parent;
    int g; // Custo real
    int h; // Heurística 
    int f; // Custo estimado (g + h)
    char move_piece;
    char move_dir;
    char *hash_key; 
} Node;

// Hash Table Entry
typedef struct HashEntry {
    char *key;
    int g;
    struct HashEntry *next;
} HashEntry;

// Priority Queue (Min-Heap)
typedef struct {
    Node **nodes;
    int size;
    int capacity;
} MinHeap;

// --- Globais ---
int TARGET_ROW = -1, TARGET_COL = -1; 
char TARGET_ID = '\0'; 
int PIECE_GROUPS[256]; 

// --- Funções Auxiliares Básicas ---

void die(const char *msg) {
    perror(msg);
    exit(1);
}

// --- VISUALIZAÇÃO GRÁFICA (CORRIGIDA - SOURCE OF TRUTH) ---
// Definida ANTES do solve para evitar erro de "implicit declaration"

void print_fancy_board(const State *s) {
    // 1. Cria um canvas temporário limpo
    char canvas[MAX_ROWS][MAX_COLS];
    
    // Preenche com pontos (vazio)
    for(int r=0; r<s->rows; r++) {
        for(int c=0; c<s->cols; c++) {
            canvas[r][c] = '.';
        }
    }

    // 2. Pinta as peças no canvas (Source of Truth)
    for(int i=0; i<s->num_pieces; i++) {
        Piece p = s->pieces[i];
        for(int r=0; r<p.h; r++) {
            for(int c=0; c<p.w; c++) {
                if (p.r + r < s->rows && p.c + c < s->cols) {
                    canvas[p.r + r][p.c + c] = p.id;
                }
            }
        }
    }

    // 3. Imprime o canvas com bordas e cores
    printf("   +");
    for(int c=0; c<s->cols; c++) printf("---");
    printf("+\n");

    for (int r = 0; r < s->rows; r++) {
        printf(" %2d¦", r); 
        for (int c = 0; c < s->cols; c++) {
            char ch = canvas[r][c]; 
            
            if (ch == '.') {
                printf(" . "); 
            } else if (ch == TARGET_ID) {
                // Vermelho
                printf("\033[1;41;37m %c \033[0m", ch); 
            } else {
                // Azul
                printf("\033[1;44;37m %c \033[0m", ch); 
            }
        }
        printf("¦\n");
    }

    printf("   +");
    for(int c=0; c<s->cols; c++) printf("---");
    printf("+\n");
}

void reconstruct_path_visual(Node *n, int step) {
    if (n->parent) {
        reconstruct_path_visual(n->parent, step - 1);
        printf("\n--- Passo %d: Mover '%c' para %c ---\n", step, n->move_piece, n->move_dir);
    } else {
        printf("\n=== Estado Inicial ===\n");
    }
    print_fancy_board(&n->state);
}

// --- Funções de Comparação e Hash ---

int compare_pieces(const void *a, const void *b) {
    const Piece *pa = (const Piece *)a;
    const Piece *pb = (const Piece *)b;
    if (pa->group_id != pb->group_id) return pa->group_id - pb->group_id;
    if (pa->r != pb->r) return pa->r - pb->r;
    return pa->c - pb->c;
}

char* generate_canonical_key(const State *s) {
    Piece temp[MAX_PIECES];
    memcpy(temp, s->pieces, sizeof(Piece) * s->num_pieces);
    qsort(temp, s->num_pieces, sizeof(Piece), compare_pieces);
    
    char *key = malloc(s->num_pieces * 20 + 10);
    char *ptr = key;
    for (int i = 0; i < s->num_pieces; i++) {
        ptr += sprintf(ptr, "%d:%d,%d|", temp[i].group_id, temp[i].r, temp[i].c);
    }
    return key;
}

int calculate_heuristic(const State *s) {
    if (TARGET_ID == '\0') return 0;
    const Piece *target = NULL;
    for (int i = 0; i < s->num_pieces; i++) {
        if (s->pieces[i].id == TARGET_ID) {
            target = &s->pieces[i];
            break;
        }
    }
    if (!target) return 10000;

    int dist = abs(target->r - TARGET_ROW) + abs(target->c - TARGET_COL);
    int penalty = 0;
    if (target->r < TARGET_ROW) {
        for (int r = target->r + target->h; r < s->rows; r++) {
            for (int c = target->c; c < target->c + target->w; c++) {
                if (s->grid[r][c] != '.' && s->grid[r][c] != TARGET_ID) {
                    penalty += 2;
                }
            }
        }
    }
    return dist + penalty;
}

// --- Estruturas de Dados (Heap e Hash) ---

MinHeap* heap_create(int capacity) {
    MinHeap *h = malloc(sizeof(MinHeap));
    h->size = 0; h->capacity = capacity;
    h->nodes = malloc(sizeof(Node*) * capacity);
    return h;
}

void heap_push(MinHeap *h, Node *n) {
    if (h->size == h->capacity) {
        h->capacity *= 2;
        h->nodes = realloc(h->nodes, sizeof(Node*) * h->capacity);
    }
    int i = h->size++;
    while (i > 0) {
        int p = (i - 1) / 2;
        if (h->nodes[p]->f <= n->f) break;
        h->nodes[i] = h->nodes[p];
        i = p;
    }
    h->nodes[i] = n;
}

Node* heap_pop(MinHeap *h) {
    if (h->size == 0) return NULL;
    Node *ret = h->nodes[0];
    Node *last = h->nodes[--h->size];
    int i = 0;
    while (i * 2 + 1 < h->size) {
        int left = i * 2 + 1, right = i * 2 + 2, small = left;
        if (right < h->size && h->nodes[right]->f < h->nodes[left]->f) small = right;
        if (last->f <= h->nodes[small]->f) break;
        h->nodes[i] = h->nodes[small];
        i = small;
    }
    h->nodes[i] = last;
    return ret;
}

HashEntry **hash_create() {
    return calloc(HASH_SIZE, sizeof(HashEntry*));
}

unsigned long hash_string(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return hash;
}

bool hash_visited(HashEntry **table, const char *key, int g) {
    unsigned long h = hash_string(key) % HASH_SIZE;
    HashEntry *e = table[h];
    while (e) {
        if (strcmp(e->key, key) == 0) {
            if (e->g <= g) return true;
            e->g = g;
            return false;
        }
        e = e->next;
    }
    HashEntry *new_e = malloc(sizeof(HashEntry));
    new_e->key = strdup(key);
    new_e->g = g;
    new_e->next = table[h];
    table[h] = new_e;
    return false;
}

// --- Mecânica do Jogo ---

bool is_valid_move(const State *s, int p_idx, int dr, int dc) {
    Piece p = s->pieces[p_idx];
    if (p.r + dr < 0 || p.r + p.h + dr > s->rows) return false;
    if (p.c + dc < 0 || p.c + p.w + dc > s->cols) return false;
    
    char my_id = p.id;
    for (int r = 0; r < p.h; r++) {
        for (int c = 0; c < p.w; c++) {
            int nr = p.r + dr + r;
            int nc = p.c + dc + c;
            char cell = s->grid[nr][nc];
            if (cell != '.' && cell != my_id) return false;
        }
    }
    return true;
}

State apply_move(const State *current, int p_idx, int dr, int dc) {
    State next = *current;
    Piece *p = &next.pieces[p_idx];
    
    for (int r = 0; r < p->h; r++) 
        for (int c = 0; c < p->w; c++) 
            next.grid[p->r + r][p->c + c] = '.';
    
    p->r += dr;
    p->c += dc;
    
    for (int r = 0; r < p->h; r++) 
        for (int c = 0; c < p->w; c++) 
            next.grid[p->r + r][p->c + c] = p->id;
    
    return next;
}

// --- Validação Defensiva (Solicitada na Imagem) ---

bool validate_input(const State *s) {
    bool valid = true;

    // a) Peça alvo presente e em local válido
    if (TARGET_ID == '\0') {
        printf("Erro de Validação: Peça alvo não identificada.\n");
        valid = false;
    } else {
        if (TARGET_ROW == -1 || TARGET_COL == -1) {
            printf("Erro de Validação: Posição final da peça alvo não encontrada.\n");
            valid = false;
        } else {
            if (TARGET_ROW < 0 || TARGET_ROW >= s->rows || 
                TARGET_COL < 0 || TARGET_COL >= s->cols) {
                printf("Erro de Validação: Coordenadas alvo (%d, %d) fora dos limites.\n",
                       TARGET_ROW, TARGET_COL);
                valid = false;
            } else {
                char cell = s->grid[TARGET_ROW][TARGET_COL];
                if (cell == '\0') { 
                     printf("Erro de Validação: Célula de destino inválida.\n");
                     valid = false;
                }
            }
        }
    }

    // a/b) Ao menos 1 célula vazia
    bool has_empty = false;
    for (int r = 0; r < s->rows; r++) {
        for (int c = 0; c < s->cols; c++) {
            if (s->grid[r][c] == '.') {
                has_empty = true;
                break;
            }
        }
    }
    if (!has_empty) {
        printf("Erro de Validação: Tabuleiro sem espaços vazios.\n");
        valid = false;
    }

    // c) Tabuleiro não está vazio
    if (s->num_pieces == 0) {
        printf("Erro de Validação: Nenhuma peça identificada.\n");
        valid = false;
    }

    // b) Peças identificadas corretamente
    for (int i = 0; i < s->num_pieces; i++) {
        if (s->pieces[i].h < 1 || s->pieces[i].w < 1) {
            printf("Erro de Validação: Peça '%c' tem dimensões inválidas.\n", s->pieces[i].id);
            valid = false;
        }
    }

    return valid;
}

// --- Parser ---

void parse_input(const char *filename, State *initial) {
    FILE *f = fopen(filename, "r");
    if (!f) die("Erro ao abrir arquivo");

    char line[256];
    int row = 0;
    int mode = 0;
    
    for(int i=0; i<256; i++) PIECE_GROUPS[i] = i; 
    initial->num_pieces = 0;
    initial->rows = 0;
    initial->cols = 0; 

    char temp_board[MAX_ROWS][MAX_COLS];

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue; 
        
        if (line[0] == '#') {
            if (strstr(line, "solu")) { mode = 1; row = 0; }
            if (strstr(line, "grupos")) mode = 2;
            continue;
        }

        if (mode == 0) { // Lendo Tabuleiro
            int current_len = strlen(line);

            if (initial->rows == 0) {
                initial->cols = current_len;
            } else {
                // Validação: Largura consistente
                if (current_len != initial->cols) {
                    fprintf(stderr, "Erro de Validação: Largura inconsistente na linha %d.\n", row + 1);
                    fclose(f);
                    exit(1);
                }
            }
            
            if (row >= MAX_ROWS || initial->cols > MAX_COLS) {
                 fprintf(stderr, "Erro: Tabuleiro excede dimensões máximas.\n");
                 exit(1);
            }

            strcpy(temp_board[row], line);
            row++;
            initial->rows = row;
        } 
        else if (mode == 1) { // Solução
            for (int c = 0; line[c]; c++) {
                if (isupper(line[c])) {
                    if (TARGET_ROW == -1) { TARGET_ROW = row; TARGET_COL = c; }
                }
            }
            row++;
        } 
        else if (mode == 2) { // Grupos
             if (isdigit(line[0])) continue;
             int group_id = -1;
             for (int i=0; line[i]; i++) {
                 if (isalpha(line[i])) {
                     if (group_id == -1) group_id = line[i];
                     PIECE_GROUPS[(unsigned char)line[i]] = group_id;
                 }
             }
        }
    }
    fclose(f);

    bool visited[MAX_ROWS][MAX_COLS] = {0};
    
    for(int r=0; r<initial->rows; r++) {
        for(int c=0; c<initial->cols; c++) initial->grid[r][c] = temp_board[r][c];
    }

    for(int r=0; r<initial->rows; r++) {
        for(int c=0; c<initial->cols; c++) {
            char id = temp_board[r][c];
            if (id != '.' && id != '0' && !visited[r][c]) {
                Piece *p = &initial->pieces[initial->num_pieces++];
                p->id = id;
                p->r = r;
                p->c = c;
                p->group_id = PIECE_GROUPS[(unsigned char)id];
                if (isupper(id)) TARGET_ID = id;

                int w = 0;
                while (c + w < initial->cols && temp_board[r][c+w] == id) w++;
                p->w = w;

                int h = 0;
                while (r + h < initial->rows && temp_board[r+h][c] == id) h++;
                p->h = h;

                for(int rr=0; rr<h; rr++)
                    for(int cc=0; cc<w; cc++)
                        visited[r+rr][c+cc] = true;
            }
        }
    }
}

// --- Main Solver ---

// Agora aceita State, não filename
void solve(State initial) { 
    printf("Resolvendo para alvo %c em (%d, %d)...\n", TARGET_ID, TARGET_ROW, TARGET_COL);

    MinHeap *pq = heap_create(10000);
    HashEntry **visited = hash_create();

    Node *root = malloc(sizeof(Node));
    root->state = initial;
    root->parent = NULL;
    root->g = 0;
    root->h = calculate_heuristic(&initial);
    root->f = root->g + root->h;
    root->hash_key = generate_canonical_key(&initial);

    heap_push(pq, root);
    hash_visited(visited, root->hash_key, 0);

    int count = 0;
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};
    char dname[] = {'U', 'D', 'L', 'R'};

    while (pq->size > 0) {
        Node *current = heap_pop(pq);
        count++;

        if (count % 10000 == 0) printf("Explorados: %d | F: %d | G: %d\n", count, current->f, current->g);

        const Piece *target = NULL;
        for(int i=0; i<current->state.num_pieces; i++) 
            if(current->state.pieces[i].id == TARGET_ID) target = &current->state.pieces[i];
        
        if (target && target->r == TARGET_ROW && target->c == TARGET_COL) {
            printf("\n?? SOLUCAO ENCONTRADA em %d movimentos! ??\n", current->g);
            reconstruct_path_visual(current, current->g);
            return;
        }

        for (int i = 0; i < current->state.num_pieces; i++) {
            for (int d = 0; d < 4; d++) {
                if (is_valid_move(&current->state, i, dr[d], dc[d])) {
                    State next_state = apply_move(&current->state, i, dr[d], dc[d]);
                    char *key = generate_canonical_key(&next_state);
                    int new_g = current->g + 1;
                    
                    if (!hash_visited(visited, key, new_g)) {
                        Node *child = malloc(sizeof(Node));
                        child->state = next_state;
                        child->parent = current;
                        child->g = new_g;
                        child->h = calculate_heuristic(&next_state);
                        child->f = child->g + child->h;
                        child->move_piece = next_state.pieces[i].id;
                        child->move_dir = dname[d];
                        child->hash_key = key;
                        heap_push(pq, child);
                    } else {
                        free(key);
                    }
                }
            }
        }
    }
    printf("Sem solucao.\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <arquivo>\n", argv[0]);
        return 1;
    }

    State initial;
    
    // 1. Lê e Preenche
    parse_input(argv[1], &initial);

    // 2. Valida
    if (!validate_input(&initial)) {
        printf("\nAbortando execução devido a erros de validação.\n");
        return 1; 
    } else {
    	printf("\nValidacao de entrada OK!.\n\n");
	}

    // 3. Resolve (passando o State já validado)
    solve(initial); 
    
    return 0;
}
