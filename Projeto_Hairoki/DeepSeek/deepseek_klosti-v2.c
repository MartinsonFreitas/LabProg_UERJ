#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// Definições do tabuleiro
#define MAX_ROWS 10
#define MAX_COLS 10
#define MAX_PIECES 26
#define MAX_MOVES 1000
#define HASH_TABLE_SIZE 1000003
#define MAX_NODES 300000

// Estruturas
typedef struct {
    char id;
    int row;
    int col;
    int height;
    int width;
    bool is_goal;
} Piece;

typedef struct {
    char board[MAX_ROWS][MAX_COLS];
    int rows;
    int cols;
    Piece pieces[MAX_PIECES];
    int num_pieces;
    char moves[MAX_MOVES];
    int move_count;
} State;

typedef struct Node {
    State state;
    struct Node *parent;
    char move_char;
    char move_dir;
    int depth;
} Node;

typedef struct Queue {
    Node *nodes[MAX_NODES];
    int front;
    int rear;
    int size;
} Queue;

typedef struct HashNode {
    int *piece_coords;
    int num_pieces;
    struct HashNode *next;
} HashNode;

typedef HashNode *HashTable[HASH_TABLE_SIZE];

// Protótipos
void print_state(const State *state);
void parse_input(const char *filename, State *initial_state);
void initialize_state(State *state);
unsigned long hash_state_optimized(const State *state);
bool is_visited(const State *state, HashTable hash_table);
void mark_visited(const State *state, HashTable hash_table);
void free_hash_table(HashTable hash_table);
bool is_goal(const State *state);
Piece *find_piece(const State *state, char id);
bool move_piece(State *current_state, char piece_id, char direction, State *next_state);
bool is_valid_move(const State *state, const Piece *p, int dr, int dc);
void reconstruct_path(Node *goal_node);
Node *create_node(const State *state, Node *parent, char move_char, char move_dir);

// Funções Queue
Queue *create_queue();
void enqueue(Queue *q, Node *node);
Node *dequeue(Queue *q);
bool is_queue_empty(Queue *q);
void free_queue(Queue *q);

// Funções otimizadas
bool is_redundant_move(const State *state, char piece_id, char direction);
bool move_piece_optimized(State *current_state, char piece_id, char direction, State *next_state);
void free_all_nodes(Queue *q, HashTable hash_table);
void solve_klotski_bfs_optimized(const char *filename);

// Implementações
void print_state(const State *state) {
    for (int i = 0; i < state->rows; i++) {
        for (int j = 0; j < state->cols; j++) {
            printf("%c", state->board[i][j]);
        }
        printf("\n");
    }
}

void initialize_state(State *state) {
    state->num_pieces = 0;
    state->move_count = 0;
    state->moves[0] = '\0';
    state->rows = 0;
    state->cols = 0;
    memset(state->board, '0', sizeof(state->board));
    memset(state->pieces, 0, sizeof(state->pieces));
}

Piece *find_piece(const State *state, char id) {
    for (int i = 0; i < state->num_pieces; i++) {
        if (state->pieces[i].id == id) {
            return (Piece *)&state->pieces[i];
        }
    }
    return NULL;
}

void parse_input(const char *filename, State *initial_state) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo de entrada");
        exit(EXIT_FAILURE);
    }

    initialize_state(initial_state);
    char line[MAX_COLS + 2];
    int row = 0;
    int max_cols = 0;
    bool reading_board = true;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        if (line[0] == '#' || line[0] == '\0') {
            if (strstr(line, "solução") != NULL) {
                reading_board = false;
            }
            continue;
        }

        if (reading_board) {
            initial_state->rows = row + 1;
            int len = strlen(line);
            if (len > max_cols) max_cols = len;
            initial_state->cols = max_cols;

            for (int col = 0; col < len; col++) {
                char id = line[col];
                initial_state->board[row][col] = id;

                if ((id >= 'a' && id <= 'z') || id == 'X') {
                    if (!find_piece(initial_state, id)) {
                        initial_state->pieces[initial_state->num_pieces].id = id;
                        initial_state->pieces[initial_state->num_pieces].row = row;
                        initial_state->pieces[initial_state->num_pieces].col = col;
                        initial_state->pieces[initial_state->num_pieces].is_goal = (id == 'X');
                        initial_state->num_pieces++;
                    }
                }
            }
            row++;
        }
    }
    fclose(file);

    // Ordenar peças por ID
    for (int i = 0; i < initial_state->num_pieces - 1; i++) {
        for (int j = i + 1; j < initial_state->num_pieces; j++) {
            if (initial_state->pieces[i].id > initial_state->pieces[j].id) {
                Piece temp = initial_state->pieces[i];
                initial_state->pieces[i] = initial_state->pieces[j];
                initial_state->pieces[j] = temp;
            }
        }
    }

    // Calcular dimensões das peças
    for (int i = 0; i < initial_state->num_pieces; i++) {
        Piece *p = &initial_state->pieces[i];
        int max_row = p->row;
        for (int r = p->row; r < initial_state->rows; r++) {
            if (initial_state->board[r][p->col] == p->id) max_row = r; else break;
        }
        p->height = max_row - p->row + 1;

        int max_col = p->col;
        for (int c = p->col; c < initial_state->cols; c++) {
            if (initial_state->board[p->row][c] == p->id) max_col = c; else break;
        }
        p->width = max_col - p->col + 1;
    }
}

bool is_goal(const State *state) {
    Piece *goal_piece = find_piece(state, 'X');
    if (!goal_piece) return false;

    if (state->rows == 5 && state->cols == 4) {
        return (goal_piece->row == 3 && goal_piece->col == 1);
    }
    return false;
}

bool is_valid_move(const State *state, const Piece *p, int dr, int dc) {
    int new_row = p->row + dr;
    int new_col = p->col + dc;

    if (new_row < 0 || new_row + p->height > state->rows ||
        new_col < 0 || new_col + p->width > state->cols) {
        return false;
    }

    for (int r = new_row; r < new_row + p->height; r++) {
        for (int c = new_col; c < new_col + p->width; c++) {
            char cell = state->board[r][c];
            if (cell != '0' && cell != '.' && cell != p->id) {
                return false;
            }
        }
    }
    return true;
}

bool move_piece(State *current_state, char piece_id, char direction, State *next_state) {
    *next_state = *current_state;
    Piece *p = find_piece(next_state, piece_id);
    if (!p) return false;

    int dr = 0, dc = 0;
    char dir_char = ' ';

    if (direction == 'U') { dr = -1; dir_char = 'C'; }
    else if (direction == 'D') { dr = 1; dir_char = 'B'; }
    else if (direction == 'L') { dc = -1; dir_char = 'E'; }
    else if (direction == 'R') { dc = 1; dir_char = 'D'; }
    else return false;

    if (!is_valid_move(current_state, p, dr, dc)) {
        return false;
    }

    int old_row = p->row, old_col = p->col;
    p->row += dr;
    p->col += dc;

    // Atualiza o tabuleiro
    for (int i = old_row; i < old_row + p->height; i++) {
        for (int j = old_col; j < old_col + p->width; j++) {
            next_state->board[i][j] = '0';
        }
    }
    for (int i = p->row; i < p->row + p->height; i++) {
        for (int j = p->col; j < p->col + p->width; j++) {
            next_state->board[i][j] = p->id;
        }
    }

    // Ordena as peças para consistência do hash
    for (int i = 0; i < next_state->num_pieces - 1; i++) {
        for (int j = i + 1; j < next_state->num_pieces; j++) {
            if (next_state->pieces[i].id > next_state->pieces[j].id) {
                Piece temp = next_state->pieces[i];
                next_state->pieces[i] = next_state->pieces[j];
                next_state->pieces[j] = temp;
            }
        }
    }

    // Atualiza a sequência de movimentos
    if (next_state->move_count < MAX_MOVES - 2) {
        next_state->moves[next_state->move_count++] = p->id;
        next_state->moves[next_state->move_count++] = dir_char;
        next_state->moves[next_state->move_count] = '\0';
    }

    return true;
}

// Queue implementations
Queue *create_queue() {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->front = 0;
    q->rear = -1;
    q->size = 0;
    return q;
}

void enqueue(Queue *q, Node *node) {
    if (q->size >= MAX_NODES) {
        return;
    }
    q->rear = (q->rear + 1) % MAX_NODES;
    q->nodes[q->rear] = node;
    q->size++;
}

Node *dequeue(Queue *q) {
    if (q->size == 0) {
        return NULL;
    }
    Node *node = q->nodes[q->front];
    q->front = (q->front + 1) % MAX_NODES;
    q->size--;
    return node;
}

bool is_queue_empty(Queue *q) {
    return q->size == 0;
}

void free_queue(Queue *q) {
    free(q);
}

// Hash otimizado
unsigned long hash_state_optimized(const State *state) {
    unsigned long hash = 5381;
    for (int i = 0; i < state->num_pieces; i++) {
        hash = ((hash << 5) + hash) + state->pieces[i].id;
        hash = ((hash << 5) + hash) + state->pieces[i].row;
        hash = ((hash << 5) + hash) + state->pieces[i].col;
    }
    return hash % HASH_TABLE_SIZE;
}

bool is_visited(const State *state, HashTable hash_table) {
    unsigned long index = hash_state_optimized(state);
    HashNode *current = hash_table[index];

    while (current != NULL) {
        if (current->num_pieces == state->num_pieces) {
            bool match = true;
            for (int i = 0; i < state->num_pieces; i++) {
                if (current->piece_coords[i * 2] != state->pieces[i].row ||
                    current->piece_coords[i * 2 + 1] != state->pieces[i].col) {
                    match = false;
                    break;
                }
            }
            if (match) {
                return true;
            }
        }
        current = current->next;
    }
    return false;
}

void mark_visited(const State *state, HashTable hash_table) {
    unsigned long index = hash_state_optimized(state);
    
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    new_node->num_pieces = state->num_pieces;
    new_node->piece_coords = (int *)malloc(state->num_pieces * 2 * sizeof(int));
    new_node->next = NULL;

    for (int i = 0; i < state->num_pieces; i++) {
        new_node->piece_coords[i * 2] = state->pieces[i].row;
        new_node->piece_coords[i * 2 + 1] = state->pieces[i].col;
    }

    new_node->next = hash_table[index];
    hash_table[index] = new_node;
}

void free_hash_table(HashTable hash_table) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode *current = hash_table[i];
        while (current != NULL) {
            HashNode *next = current->next;
            free(current->piece_coords);
            free(current);
            current = next;
        }
        hash_table[i] = NULL;
    }
}

Node *create_node(const State *state, Node *parent, char move_char, char move_dir) {
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->state = *state;
    temp->parent = parent;
    temp->move_char = move_char;
    temp->move_dir = move_dir;
    temp->depth = (parent ? parent->depth + 1 : 0);
    return temp;
}

void reconstruct_path(Node *goal_node) {
    if (!goal_node) return;
    
    char path[MAX_MOVES * 2];
    path[0] = '\0';
    int len = 0;
    Node *current = goal_node;
    
    while (current->parent != NULL) {
        path[len++] = current->move_char;
        path[len++] = current->move_dir;
        current = current->parent;
    }
    
    // Inverter o caminho
    for (int i = 0; i < len / 2; i++) {
        char temp_char = path[i * 2];
        char temp_dir = path[i * 2 + 1];
        path[i * 2] = path[len - 2 - i * 2];
        path[i * 2 + 1] = path[len - 1 - i * 2];
        path[len - 2 - i * 2] = temp_char;
        path[len - 1 - i * 2] = temp_dir;
    }
    path[len] = '\0';

    printf("Solução encontrada em %d movimentos (BFS):\n", goal_node->depth);
    printf("%s\n", path);
}

// Funções otimizadas
bool is_redundant_move(const State *state, char piece_id, char direction) {
    if (state->move_count < 2) return false;
    
    char last_piece = state->moves[state->move_count - 2];
    char last_dir = state->moves[state->move_count - 1];
    
    if (last_piece == piece_id) {
        if ((last_dir == 'C' && direction == 'B') ||
            (last_dir == 'B' && direction == 'C') ||
            (last_dir == 'E' && direction == 'D') ||
            (last_dir == 'D' && direction == 'E')) {
            return true;
        }
    }
    
    return false;
}

bool move_piece_optimized(State *current_state, char piece_id, char direction, State *next_state) {
    if (is_redundant_move(current_state, piece_id, direction)) {
        return false;
    }
    
    return move_piece(current_state, piece_id, direction, next_state);
}

void free_all_nodes(Queue *q, HashTable hash_table) {
    // Libera nós da queue
    while (!is_queue_empty(q)) {
        Node *node = dequeue(q);
        if (node) free(node);
    }
    
    // Libera hash table
    free_hash_table(hash_table);
}

// ESTRATÉGIA OTIMIZADA: Foca primeiro nos movimentos da peça 'X' e peças próximas
void solve_klotski_bfs_optimized(const char *filename) {
    State initial_state;
    parse_input(filename, &initial_state);

    printf("Estado Inicial:\n");
    print_state(&initial_state);

    if (is_goal(&initial_state)) {
        printf("O estado inicial já é o estado final.\n");
        return;
    }

    Queue *q = create_queue();
    HashTable visited = {0};
    
    Node *initial_node = create_node(&initial_state, NULL, '\0', '\0');
    enqueue(q, initial_node);
    mark_visited(&initial_state, visited);

    Node *goal_node = NULL;
    int nodes_expanded = 0;

    while (!is_queue_empty(q)) {
        Node *current_node = dequeue(q);
        State *current_state = &current_node->state;
        nodes_expanded++;

        if (nodes_expanded % 10000 == 0) {
            printf("Nós expandidos: %d, Profundidade: %d, Fila: %d\n", 
                   nodes_expanded, current_node->depth, q->size);
        }

        if (is_goal(current_state)) {
            goal_node = current_node;
            break;
        }

        // ESTRATÉGIA: Tentar primeiro mover a peça 'X' e peças que a bloqueiam
        for (int i = 0; i < current_state->num_pieces; i++) {
            char piece_id = current_state->pieces[i].id;
            if (piece_id == '0' || piece_id == '.') continue;

            // Prioridade: peça 'X' primeiro, depois outras peças
            char directions[] = {'U', 'D', 'L', 'R'};
            for (int d = 0; d < 4; d++) {
                State next_state;
                if (move_piece_optimized(current_state, piece_id, directions[d], &next_state)) {
                    
                    if (!is_visited(&next_state, visited)) {
                        Node *next_node = create_node(&next_state, current_node, piece_id, directions[d]);
                        enqueue(q, next_node);
                        mark_visited(&next_state, visited);
                    }
                }
            }
        }
    }

    if (goal_node) {
        reconstruct_path(goal_node);
        printf("Nós expandidos totais: %d\n", nodes_expanded);
    } else {
        printf("Solução não encontrada após expandir %d nós.\n", nodes_expanded);
    }

    free_all_nodes(q, visited);
    free_queue(q);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("Resolvendo Klotski com BFS otimizado...\n");
    solve_klotski_bfs_optimized(argv[1]);

    return EXIT_SUCCESS;
}
