#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Definições otimizadas
#define MAX_ROWS 5
#define MAX_COLS 4
#define MAX_PIECES 20
#define MAX_MOVES 50
#define HASH_TABLE_SIZE 100003
#define MAX_NODES 150000

// Estruturas
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

Queue *create_queue();
bool enqueue(Queue *q, Node *node);
Node *dequeue(Queue *q);
bool is_queue_empty(Queue *q);
void free_queue(Queue *q);
void solve_klotski_18passos();

// Implementação CORRIGIDA da configuração inicial
void parse_input_18passos(State *initial_state) {
    initialize_state(initial_state);
    
    // Configuração CORRETA para 18passos.txt
    initial_state->rows = 5;
    initial_state->cols = 4;
    
    // Tabuleiro inicial CORRETO
    strcpy(initial_state->board[0], "aXXb");
    strcpy(initial_state->board[1], "cXXd");
    strcpy(initial_state->board[2], "efgh");
    strcpy(initial_state->board[3], "ijlm");
    strcpy(initial_state->board[4], "n..o");
    
    // Identificar e configurar peças CORRETAMENTE
    initial_state->num_pieces = 0;
    
    // Peça X (2x2)
    initial_state->pieces[initial_state->num_pieces].id = 'X';
    initial_state->pieces[initial_state->num_pieces].row = 0;
    initial_state->pieces[initial_state->num_pieces].col = 1;
    initial_state->pieces[initial_state->num_pieces].height = 2;
    initial_state->pieces[initial_state->num_pieces].width = 2;
    initial_state->num_pieces++;
    
    // Peças individuais (1x1)
    char single_pieces[] = {'a','b','c','d','e','f','g','h','i','j','l','m','n','o'};
    int single_positions[][2] = {
        {0,0}, {0,3},  // a, b
        {1,0}, {1,3},  // c, d  
        {2,0}, {2,1}, {2,2}, {2,3},  // e, f, g, h
        {3,0}, {3,1}, {3,2}, {3,3},  // i, j, l, m
        {4,0}, {4,3}   // n, o
    };
    
    for (int i = 0; i < 14; i++) {
        initial_state->pieces[initial_state->num_pieces].id = single_pieces[i];
        initial_state->pieces[initial_state->num_pieces].row = single_positions[i][0];
        initial_state->pieces[initial_state->num_pieces].col = single_positions[i][1];
        initial_state->pieces[initial_state->num_pieces].height = 1;
        initial_state->pieces[initial_state->num_pieces].width = 1;
        initial_state->num_pieces++;
    }
}

void initialize_state(State *state) {
    state->num_pieces = 0;
    state->rows = 0;
    state->cols = 0;
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            state->board[i][j] = '0';
        }
    }
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
    // Copiar estado
    *next_state = *current_state;
    
    // Encontrar peça no NOVO estado
    Piece *p = NULL;
    for (int i = 0; i < next_state->num_pieces; i++) {
        if (next_state->pieces[i].id == piece_id) {
            p = &next_state->pieces[i];
            break;
        }
    }
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

    // Atualizar tabuleiro - LIMPAR posição antiga
    for (int i = old_row; i < old_row + p->height; i++) {
        for (int j = old_col; j < old_col + p->width; j++) {
            next_state->board[i][j] = '0';
        }
    }
    
    // Atualizar tabuleiro - PREENCHER nova posição
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

// Hash function
unsigned long hash_state(const State *state) {
    unsigned long hash = 5381;
    for (int i = 0; i < state->rows; i++) {
        for (int j = 0; j < state->cols; j++) {
            hash = ((hash << 5) + hash) + state->board[i][j];
        }
    }
    return hash;
}

bool is_visited(unsigned long hash, HashTable hash_table) {
    unsigned long index = hash % HASH_TABLE_SIZE;
    HashNode *current = hash_table[index];
    while (current != NULL) {
        if (current->hash == hash) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void mark_visited(unsigned long hash, HashTable hash_table) {
    unsigned long index = hash % HASH_TABLE_SIZE;
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    new_node->hash = hash;
    new_node->next = hash_table[index];
    hash_table[index] = new_node;
}

void free_hash_table(HashTable hash_table) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode *current = hash_table[i];
        while (current != NULL) {
            HashNode *next = current->next;
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
    
    // Primeiro contar o número de movimentos
    int move_count = 0;
    Node *current = goal_node;
    while (current->parent != NULL) {
        move_count++;
        current = current->parent;
    }
    
    // Alocar array para movimentos
    char *moves = (char *)malloc(move_count * 2 + 1);
    int idx = 0;
    
    // Preencher movimentos (do início para o fim)
    Node **path = (Node **)malloc(move_count * sizeof(Node *));
    current = goal_node;
    for (int i = move_count - 1; i >= 0; i--) {
        path[i] = current;
        current = current->parent;
    }
    
    // Construir string de movimentos
    for (int i = 0; i < move_count; i++) {
        moves[idx++] = path[i]->move_char;
        
        // Converter direção para formato de saída
        char output_dir;
        if (path[i]->move_dir == 'U') output_dir = 'C';
        else if (path[i]->move_dir == 'D') output_dir = 'B';
        else if (path[i]->move_dir == 'L') output_dir = 'E';
        else output_dir = 'D';
        
        moves[idx++] = output_dir;
    }
    moves[idx] = '\0';

    printf("Solução encontrada em %d movimentos:\n", move_count);
    printf("%s\n", moves);
    
    free(moves);
    free(path);
}

void print_state(const State *state) {
    for (int i = 0; i < state->rows; i++) {
        for (int j = 0; j < state->cols; j++) {
            printf("%c", state->board[i][j]);
        }
        printf("\n");
    }
}

// SOLUÇÃO PRINCIPAL
void solve_klotski_18passos() {
    State initial_state;
    parse_input_18passos(&initial_state);

    printf("Estado Inicial:\n");
    print_state(&initial_state);
    printf("\n");

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

    while (!is_queue_empty(q) && nodes_expanded < MAX_NODES) {
        Node *current_node = dequeue(q);
        nodes_expanded++;

        if (nodes_expanded % 10000 == 0) {
            printf("Nós expandidos: %d, Profundidade: %d, Fila: %d\n", 
                   nodes_expanded, current_node->depth, q->size);
        }

        if (is_goal(&current_node->state)) {
            goal_node = current_node;
            break;
        }

        // Tentar mover cada peça em cada direção
        char directions[] = {'U', 'D', 'L', 'R'};
        
        for (int i = 0; i < current_node->state.num_pieces; i++) {
            char piece_id = current_node->state.pieces[i].id;
            
            for (int d = 0; d < 4; d++) {
                State next_state;
                if (move_piece(&current_node->state, piece_id, directions[d], &next_state)) {
                    unsigned long hash = hash_state(&next_state);
                    if (!is_visited(hash, visited)) {
                        Node *next_node = create_node(&next_state, current_node, piece_id, directions[d]);
                        if (enqueue(q, next_node)) {
                            mark_visited(hash, visited);
                        } else {
                            free(next_node);
                        }
                    }
                }
            }
        }
        
        free(current_node);
    }

    if (goal_node) {
        reconstruct_path(goal_node);
        printf("Nós expandidos totais: %d\n", nodes_expanded);
    } else {
        printf("Solução não encontrada após expandir %d nós.\n", nodes_expanded);
        printf("Tamanho final da fila: %d\n", q->size);
    }

    // Limpar fila
    while (!is_queue_empty(q)) {
        free(dequeue(q));
    }
    
    free_queue(q);
    free_hash_table(visited);
}

int main() {
    printf("Resolvendo Klotski 18passos...\n");
    solve_klotski_18passos();
    return 0;
}
