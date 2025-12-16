#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// --- Definições ---
#define MAX_ROWS 10
#define MAX_COLS 10
#define MAX_PIECES 26
#define HASH_TABLE_SIZE 200003 // Primo para hash
#define MAX_NODES 2000000      // Aumentado o limite seguro

// --- Estruturas ---

typedef struct {
    char id;
    int row;
    int col;
    int height;
    int width;
    bool is_goal_piece; // Flag para identificar a peça alvo (Maiúscula)
} Piece;

typedef struct {
    char board[MAX_ROWS][MAX_COLS];
    int rows;
    int cols;
    Piece pieces[MAX_PIECES];
    int num_pieces;
    // REMOVIDO: char moves[MAX_MOVES]; <- Causa do "Morto"
} State;

typedef struct Node {
    State state;
    struct Node *parent;
    char move_char; // Peça que moveu para chegar aqui
    char move_dir;  // Direção
    int g;          // Custo real
    int h;          // Heurística
    int f;          // g + h
} Node;

// Heap Binário
typedef struct {
    Node **nodes;
    int size;
    int capacity;
} PriorityQueue;

// Hash Table para visitados
typedef struct HashEntry {
    unsigned long hash;
    State state; // Armazena estado para colisão
    int g;       // Para verificar se encontramos caminho melhor
    struct HashEntry *next;
} HashEntry;

typedef struct {
    HashEntry **buckets;
} HashTable;

// Variáveis Globais para o Objetivo (Lidas do arquivo)
int TARGET_ROW = -1;
int TARGET_COL = -1;

// --- Protótipos ---
unsigned long hash_state(const State *s);
int calculate_heuristic(const State *s);

// --- Implementação da Priority Queue (Dinâmica) ---

PriorityQueue *pq_create() {
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    pq->capacity = 10000; // Começa pequeno
    pq->size = 0;
    pq->nodes = malloc(sizeof(Node*) * pq->capacity);
    return pq;
}

void pq_push(PriorityQueue *pq, Node *node) {
    if (pq->size == pq->capacity) {
        pq->capacity *= 2;
        pq->nodes = realloc(pq->nodes, sizeof(Node*) * pq->capacity);
    }
    int i = pq->size++;
    while (i > 0) {
        int p = (i - 1) / 2;
        if (pq->nodes[p]->f <= node->f) break;
        pq->nodes[i] = pq->nodes[p];
        i = p;
    }
    pq->nodes[i] = node;
}

Node *pq_pop(PriorityQueue *pq) {
    if (pq->size == 0) return NULL;
    Node *ret = pq->nodes[0];
    Node *last = pq->nodes[--pq->size];
    int i = 0;
    while (i * 2 + 1 < pq->size) {
        int left = i * 2 + 1;
        int right = i * 2 + 2;
        int smaller = left;
        if (right < pq->size && pq->nodes[right]->f < pq->nodes[left]->f)
            smaller = right;
        if (last->f <= pq->nodes[smaller]->f) break;
        pq->nodes[i] = pq->nodes[smaller];
        i = smaller;
    }
    pq->nodes[i] = last;
    return ret;
}

// --- Funções de Estado e Auxiliares ---

Piece* get_piece(State *s, char id) {
    for(int i=0; i<s->num_pieces; i++) {
        if(s->pieces[i].id == id) return &s->pieces[i];
    }
    return NULL;
}

// Verifica se chegou ao estado objetivo lido do arquivo
bool is_goal(const State *s) {
    // Procura a peça marcada como alvo (Maiúscula)
    for (int i = 0; i < s->num_pieces; i++) {
        if (s->pieces[i].is_goal_piece) {
            // Verifica se a posição coincide com o alvo lido do arquivo
            return (s->pieces[i].row == TARGET_ROW && s->pieces[i].col == TARGET_COL);
        }
    }
    return false;
}

// Heurística: Distância de Manhattan da peça alvo até o TARGET
int calculate_heuristic(const State *s) {
    for (int i = 0; i < s->num_pieces; i++) {
        if (s->pieces[i].is_goal_piece) {
            return abs(s->pieces[i].row - TARGET_ROW) + abs(s->pieces[i].col - TARGET_COL);
        }
    }
    return 10000;
}

// Hash usando DJB2 nos dados brutos do tabuleiro para velocidade
unsigned long hash_state(const State *s) {
    unsigned long hash = 5381;
    for (int i = 0; i < s->rows; i++) {
        for (int j = 0; j < s->cols; j++) {
             hash = ((hash << 5) + hash) + s->board[i][j];
        }
    }
    return hash;
}

// --- Hash Table para Visitados ---

HashTable* ht_create() {
    HashTable *ht = malloc(sizeof(HashTable));
    ht->buckets = calloc(HASH_TABLE_SIZE, sizeof(HashEntry*));
    return ht;
}

// Retorna -1 se não existe, ou o valor 'g' se já existe
int ht_check_and_update(HashTable *ht, State *s, int g) {
    unsigned long h = hash_state(s);
    int idx = h % HASH_TABLE_SIZE;
    
    HashEntry *e = ht->buckets[idx];
    while (e) {
        // Verifica colisão comparando o tabuleiro memória a memória
        if (memcmp(e->state.board, s->board, sizeof(s->board)) == 0) {
            if (g < e->g) {
                e->g = g; // Encontramos um caminho melhor para o mesmo estado
                return 1; // Atualizar
            }
            return 0; // Já visitado com custo menor ou igual
        }
        e = e->next;
    }
    
    // Novo estado
    HashEntry *new_entry = malloc(sizeof(HashEntry));
    new_entry->hash = h;
    new_entry->state = *s;
    new_entry->g = g;
    new_entry->next = ht->buckets[idx];
    ht->buckets[idx] = new_entry;
    return -1; // Novo
}

// --- Leitura do Arquivo (Parser Robusto) ---

void parse_input(const char *filename, State *initial_state) {
    FILE *f = fopen(filename, "r");
    if (!f) { perror("Erro arquivo"); exit(1); }

    char line[128];
    int row = 0;
    int mode = 0; // 0=Tabuleiro, 1=Solução, 2=Grupos
    
    initial_state->num_pieces = 0;
    memset(initial_state->board, '.', sizeof(initial_state->board));

    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#' || line[0] == '\n') {
            if (strstr(line, "solução")) { mode = 1; row = 0; }
            if (strstr(line, "grupos")) mode = 2;
            continue;
        }

        int len = strlen(line);
        if (line[len-1] == '\n') line[--len] = 0;

        if (mode == 0) { // Lendo Tabuleiro Inicial
            if (row == 0) initial_state->cols = len;
            initial_state->rows = row + 1;
            
            for (int c = 0; c < len; c++) {
                char ch = line[c];
                initial_state->board[row][c] = ch;
                if (ch != '.' && ch != '0') {
                    // Verifica se peça já existe
                    Piece *p = get_piece(initial_state, ch);
                    if (!p) {
                        p = &initial_state->pieces[initial_state->num_pieces++];
                        p->id = ch;
                        p->row = row; 
                        p->col = c;
                        p->height = 1; p->width = 1;
                        // Maiúscula = Alvo [cite: 9]
                        p->is_goal_piece = (ch >= 'A' && ch <= 'Z');
                    } else {
                        // Atualiza dimensões
                        if (row > p->row) p->height = (row - p->row) + 1;
                        if (c > p->col) p->width = (c - p->col) + 1;
                    }
                }
            }
            row++;
        }
        else if (mode == 1) { // Lendo Solução para pegar o ALVO
            for (int c = 0; c < len; c++) {
                if (line[c] >= 'A' && line[c] <= 'Z') {
                    // Encontrou a peça alvo na solução
                    if (TARGET_ROW == -1) { // Pega apenas o topo-esquerda
                        TARGET_ROW = row;
                        TARGET_COL = c;
                    }
                }
            }
            row++;
        }
    }
    fclose(f);
}

// --- Mecânica de Movimento ---

bool is_valid(State *s, Piece *p, int dr, int dc) {
    // Limites do tabuleiro
    if (p->row + dr < 0 || p->row + p->height + dr > s->rows) return false;
    if (p->col + dc < 0 || p->col + p->width + dc > s->cols) return false;

    // Colisões
    // Removemos a peça temporariamente do tabuleiro lógico para verificar colisão
    char temp_id = p->id;
    
    // Verifica as células de destino
    for (int r = 0; r < p->height; r++) {
        for (int c = 0; c < p->width; c++) {
            // Célula que a peça vai ocupar
            int nr = p->row + dr + r;
            int nc = p->col + dc + c;
            
            char cell = s->board[nr][nc];
            // Se a célula não for vazia e não for a própria peça
            if (cell != '.' && cell != temp_id && cell != '0') return false;
        }
    }
    return true;
}

void apply_move(State *s, Piece *p, int dr, int dc) {
    // Limpa posição antiga
    for(int r=0; r<p->height; r++)
        for(int c=0; c<p->width; c++)
            s->board[p->row + r][p->col + c] = '.'; // Assume ponto como vazio

    p->row += dr;
    p->col += dc;

    // Preenche nova posição
    for(int r=0; r<p->height; r++)
        for(int c=0; c<p->width; c++)
            s->board[p->row + r][p->col + c] = p->id;
}

// --- Solver ---

void reconstruct_path(Node *node) {
    if (!node->parent) return;
    reconstruct_path(node->parent);
    printf("%c %c\n", node->move_char, node->move_dir);
}

void solve(const char *filename) {
    State initial;
    parse_input(filename, &initial);
    
    if (TARGET_ROW == -1) {
        printf("Erro: Não foi possível identificar a posição final da peça alvo no arquivo.\n");
        return;
    }

    PriorityQueue *pq = pq_create();
    HashTable *ht = ht_create();

    Node *root = malloc(sizeof(Node));
    root->state = initial;
    root->parent = NULL;
    root->g = 0;
    root->h = calculate_heuristic(&initial);
    root->f = root->g + root->h;
    
    pq_push(pq, root);
    ht_check_and_update(ht, &initial, 0);

    int nodes_expanded = 0;

    while (pq->size > 0) {
        Node *current = pq_pop(pq);
        
        if (is_goal(&current->state)) {
            printf("Solução encontrada em %d passos.\n", current->g);
            reconstruct_path(current);
            // Cleanup seria ideal aqui, mas o SO vai limpar ao sair
            return;
        }

        nodes_expanded++;
        if (nodes_expanded % 10000 == 0) {
            // Feedback visual para saber que não travou
            // printf("Nós expandidos: %d | Fila: %d\n", nodes_expanded, pq->size); 
        }

        // Gera sucessores
        State *s = &current->state;
        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};
        char dir_char[] = {'C', 'B', 'E', 'D'}; // Cima, Baixo, Esquerda, Direita

        for (int i = 0; i < s->num_pieces; i++) {
            Piece *p = &s->pieces[i];
            
            for (int d = 0; d < 4; d++) {
                if (is_valid(s, p, dr[d], dc[d])) {
                    
                    Node *child = malloc(sizeof(Node));
                    child->state = *s; // Copia estado
                    
                    // Atualiza ponteiro da peça na cópia (pois o ponteiro p aponta para o pai)
                    Piece *child_p = &child->state.pieces[i]; 
                    apply_move(&child->state, child_p, dr[d], dc[d]);

                    child->g = current->g + 1;
                    child->h = calculate_heuristic(&child->state);
                    child->f = child->g + child->h;
                    child->parent = current;
                    child->move_char = child_p->id;
                    child->move_dir = dir_char[d];

                    // Verifica visitados
                    int status = ht_check_and_update(ht, &child->state, child->g);
                    if (status == -1) { // Novo estado
                        pq_push(pq, child);
                    } else {
                        free(child); // Estado repetido ou pior
                    }
                }
            }
        }
    }
    printf("Sem solução encontrada.\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <arquivo>\n", argv[0]);
        return 1;
    }
    solve(argv[1]);
    return 0;
}
