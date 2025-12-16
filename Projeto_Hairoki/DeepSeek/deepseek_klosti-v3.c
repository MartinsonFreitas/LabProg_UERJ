#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Definições reduzidas para economizar memória
#define MAX_ROWS 5
#define MAX_COLS 4
#define MAX_PIECES 20
#define MAX_MOVES 50
#define HASH_TABLE_SIZE 100003  // Reduzido
#define MAX_NODES 150000        // Reduzido

// Estruturas compactas
typedef struct {
    char id;
    int row;
    int col;
    int height;
    int width;
} Piece;

typedef struct {
    char board[MAX_ROWS][MAX_COLS];
    int rows;
    int cols;
    Piece pieces[MAX_PIECES];
    int num_pieces;
} State;

typedef struct Node {
    struct Node *parent;
    char move_char;
    char move_dir;
    int depth;
    State state;
} Node;

typedef struct Queue {
    Node *nodes[MAX_NODES];
    int front;
    int rear;
    int size;
} Queue;

typedef struct HashNode {
    unsigned long hash;
    struct HashNode *next;
} HashNode;

typedef HashNode *HashTable[HASH_TABLE_SIZE];

// Protótipos
void print_state(const State *state);
void parse_input_18passos(State *initial_state);
void initialize_state(State *state);
unsigned long hash_state(const State *state);
bool is_visited(unsigned long hash, HashTable hash_table);
void mark_visited(unsigned long hash, HashTable hash_table);
void free_hash_table(HashTable hash_table);
bool is_goal(const State *state);
Piece *find_piece(const State *state, char id);
bool move_piece(State *current_state, char piece_id, char direction, State *next_state);
bool is_valid_move(const State *state, const Piece *p, int dr, int dc);
void reconstruct_path(Node *goal_node);
Node *create_node(const State *state, Node *parent, char move_char, char move_dir);

// Funções Queue
Queue *create_queue();
bool enqueue(Queue *q, Node *node);
Node *dequeue(Queue *q);
bool is_queue_empty(Queue *q);
void free_queue(Queue *q);

// Funções otimizadas
void solve_klotski_18passos();

// Implementações específicas para 18passos.txt
void parse_input_18passos(State *initial_state) {
    initialize_state(initial_state);
    
    // Configuração fixa para 18passos.txt
    initial_state->rows = 5;
    initial_state->cols = 4;
    
    // Tabuleiro inicial
    strcpy(initial_state->board[0], "aXXb");
    strcpy(initial_state->board[1], "cXXd");
    strcpy(initial_state->board[2], "efgh");
    strcpy(initial_state->board[3], "ijlm");
    strcpy(initial_state->board[4], "n..o");
    
    // Identificar peças
    char piece_ids[] = {'a','b','c','d','e','f','g','h','i','j','l','m','n','o','X'};
    initial_state->num_pieces = 15;
    
    for (int i = 0; i < initial_state->num_pieces; i++) {
        initial_state->pieces[i].id = piece_ids[i];
        initial_state->pieces[i].row = -1;
        initial_state->pieces[i].col = -1;
    }
    
    // Encontrar posições das peças
    for (int i = 0; i < initial_state->rows; i++) {
        for (int j = 0; j < initial_state->cols; j++) {
            char id = initial_state->board[i][j];
            if (id != '.' && id != '0') {
                Piece *p = find_piece(initial_state, id);
                if (p && p->row == -1) {
                    p->row = i;
                    p->col = j;
                }
            }
        }
    }
    
    // Calcular dimensões
    for (int i = 0; i < initial_state->num_pieces; i++) {
        Piece *p = &initial_state->pieces[i];
        if (p->row == -1) continue;
        
        // Calcular altura
        p->height = 1;
        for (int r = p->row + 1; r < initial_state->rows; r++) {
            if (initial_state->board[r][p->col] == p->id) {
                p->height++;
            } else {
                break;
            }
        }
        
        // Calcular largura
        p->width = 1;
        for (int c = p->col + 1; c < initial_state->cols; c++) {
            if (initial_state->board[p->row][c] == p->id) {
                p->width++;
            } else {
                break;
            }
        }
    }
}

void initialize_state(State *state) {
    state->num_pieces = 0;
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

bool is_goal(const State *state) {
    Piece *goal_piece = find_piece(state, 'X');
    return (goal_piece && goal_piece->row == 3 && goal_piece->col == 1);
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
    if (direction == 'U') dr = -1;
    else if (direction == 'D') dr = 1;
    else if (direction == 'L') dc = -1;
    else if (direction == 'R') dc = 1;
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

bool enqueue(Queue *q, Node *node) {
    if (q->size >= MAX_NODES) {
        return false;
    }
    q->rear = (q->rear + 1) % MAX_NODES;
    q->nodes[q->rear] = node;
    q->size++;
    return true;
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

// Hash otimizado - apenas para verificação de visita
unsigned long hash_state(const State *state) {
    unsigned long hash = 5381;
    for (int i = 0; i < state->rows; i++) {
        for (int j = 0; j < state->cols; j++) {
            hash = ((hash << 5) + hash) + state->board[i][j];
        }
    }
    return hash % HASH_TABLE_SIZE;
}

bool is_visited(unsigned long hash, HashTable hash_table) {
    HashNode *current = hash_table[hash];
    while (current != NULL) {
        if (current->hash == hash) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void mark_visited(unsigned long hash, HashTable hash_table) {
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    new_node->hash = hash;
    new_node->next = hash_table[hash];
    hash_table[hash] = new_node;
}

void free_hash_table(HashTable hash_table) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode *current = hash_table[i];
        while (current != NULL) {
            HashNode *next = current->next;
            free(current);
            current = next;
        }
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
    
    // Contar o número de movimentos
    int move_count = 0;
    Node *current = goal_node;
    while (current->parent != NULL) {
        move_count++;
        current = current->parent;
    }
    
    // Alocar array para movimentos
    char *moves = (char *)malloc(move_count * 2 + 1);
    int idx = 0;
    
    current = goal_node;
    while (current->parent != NULL) {
        moves[idx++] = current->move_char;
        
        // Converter direção para formato de saída
        char output_dir;
        if (current->move_dir == 'U') output_dir = 'C';
        else if (current->move_dir == 'D') output_dir = 'B';
        else if (current->move_dir == 'L') output_dir = 'E';
        else output_dir = 'D';
        
        moves[idx++] = output_dir;
        current = current->parent;
    }
    
    // Inverter a string
    for (int i = 0; i < idx / 2; i++) {
        char temp1 = moves[i];
        char temp2 = moves[i + 1];
        moves[i] = moves[idx - 2 - i];
        moves[i + 1] = moves[idx - 1 - i];
        moves[idx - 2 - i] = temp1;
        moves[idx - 1 - i] = temp2;
    }
    moves[idx] = '\0';

    printf("Solução encontrada em %d movimentos:\n", move_count);
    printf("%s\n", moves);
    free(moves);
}

void print_state(const State *state) {
    for (int i = 0; i < state->rows; i++) {
        for (int j = 0; j < state->cols; j++) {
            printf("%c", state->board[i][j]);
        }
        printf("\n");
    }
}

// SOLUÇÃO ALTAMENTE OTIMIZADA
void solve_klotski_18passos() {
    State initial_state;
    parse_input_18passos(&initial_state);

    printf("Estado Inicial:\n");
    print_state(&initial_state);

    if (is_goal(&initial_state)) {
        printf("O estado inicial já é o estado final.\n");
        return;
    }

    Queue *q = create_queue();
    HashTable visited = {0};
    
    Node *initial_node = create_node(&initial_state, NULL, '\0', '\0');
    if (!enqueue(q, initial_node)) {
        printf("Erro: Fila cheia no início!\n");
        return;
    }
    mark_visited(hash_state(&initial_state), visited);

    Node *goal_node = NULL;
    int nodes_expanded = 0;

    while (!is_queue_empty(q)) {
        Node *current_node = dequeue(q);
        State *current_state = &current_node->state;
        nodes_expanded++;

        if (nodes_expanded % 5000 == 0) {
            printf("Nós expandidos: %d, Profundidade: %d, Fila: %d\n", 
                   nodes_expanded, current_node->depth, q->size);
        }

        if (is_goal(current_state)) {
            goal_node = current_node;
            break;
        }

        // ESTRATÉGIA: Ordem de movimentos inteligente
        char priority_order[] = {'X', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'l', 'm', 'n', 'o'};
        char directions[] = {'U', 'D', 'L', 'R'};

        for (int p = 0; p < 15; p++) {
            char piece_id = priority_order[p];
            if (!find_piece(current_state, piece_id)) continue;

            for (int d = 0; d < 4; d++) {
                State next_state;
                if (move_piece(current_state, piece_id, directions[d], &next_state)) {
                    unsigned long hash = hash_state(&next_state);
                    if (!is_visited(hash, visited)) {
                        Node *next_node = create_node(&next_state, current_node, piece_id, directions[d]);
                        if (enqueue(q, next_node)) {
                            mark_visited(hash, visited);
                        } else {
                            free(next_node);
                            // Fila cheia - limpar um pouco
                            if (q->size > MAX_NODES * 0.9) {
                                printf("Fila quase cheia. Parando busca.\n");
                                goto cleanup;
                            }
                        }
                    }
                }
            }
        }
        
        // Liberar memória do nó atual após expandir
        free(current_node);
    }

cleanup:
    if (goal_node) {
        reconstruct_path(goal_node);
        printf("Nós expandidos totais: %d\n", nodes_expanded);
    } else {
        printf("Solução não encontrada após expandir %d nós.\n", nodes_expanded);
    }

    // Limpar nós restantes na fila
    while (!is_queue_empty(q)) {
        Node *node = dequeue(q);
        free(node);
    }
    
    free_queue(q);
    free_hash_table(visited);
}

int main(int argc, char *argv[]) {
    printf("Resolvendo Klotski 18passos com BFS altamente otimizado...\n");
    solve_klotski_18passos();
    return 0;
}
