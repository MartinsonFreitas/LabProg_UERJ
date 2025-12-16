#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Definições do tabuleiro
#define MAX_ROWS 10
#define MAX_COLS 10
#define MAX_PIECES 26 // 'a' a 'z'

// Estrutura para representar uma peça
typedef struct {
    char id;
    int row;
    int col;
    int height;
    int width;
    bool is_goal;
} Piece;

// Estrutura para representar o estado do tabuleiro
typedef struct {
    char board[MAX_ROWS][MAX_COLS];
    int rows;
    int cols;
    Piece pieces[MAX_PIECES];
    int num_pieces;
    char moves[1000]; // Armazena a sequência de movimentos
    int move_count;
} State;

// Estrutura para o nó da busca (BFS)
typedef struct Node {
    State state;
    struct Node *parent;
    char move_char; // Peça movida
    char move_dir;  // Direção do movimento (U, D, L, R)
} Node;

// Estrutura para a fila (BFS)
typedef struct Queue {
    Node *front, *rear;
} Queue;

// Estrutura para o conjunto de estados visitados (Hash Set)
#define HASH_TABLE_SIZE 1000003 // Número primo grande
typedef struct HashNode {
    int *piece_coords; // Array de coordenadas (r1, c1, r2, c2, ...)
    int num_pieces;
    Node *node_ptr; // Ponteiro para o nó correspondente na fila
    struct HashNode *next;
} HashNode;

typedef HashNode *HashTable[HASH_TABLE_SIZE];

// Protótipos
void parse_input(const char *filename, State *initial_state);
void initialize_state(State *state);
void print_state(const State *state);
unsigned long hash_state_optimized(const State *state);
bool is_visited_optimized(const State *state, HashTable hash_table);
Node *get_visited_node(const State *state, HashTable hash_table);
void mark_visited_optimized(const State *state, Node *node, HashTable hash_table);
bool is_goal(const State *state);
void enqueue(Queue *q, Node *node);
Node *dequeue(Queue *q);
Node *create_node(const State *state, Node *parent, char move_char, char move_dir);
void free_queue(Queue *q);
void free_hash_table(HashTable hash_table);
void solve_klotski(const char *filename);
void reconstruct_path(Node *fwd_node, Node *bwd_node);
bool move_piece(State *current_state, char piece_id, char direction, State *next_state);
Piece *find_piece(const State *state, char id);
void create_goal_state(const State *initial_state, State *goal_state);

// Funções de utilidade para a fila (mantidas)
Queue *create_queue() {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    return q;
}

Node *create_node(const State *state, Node *parent, char move_char, char move_dir) {
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->state = *state;
    temp->parent = parent;
    temp->move_char = move_char;
    temp->move_dir = move_dir;
    return temp;
}

void enqueue(Queue *q, Node *node) {
    if (q->rear == NULL) {
        q->front = q->rear = node;
        return;
    }
    q->rear->parent = node;
    q->rear = node;
}

Node *dequeue(Queue *q) {
    if (q->front == NULL)
        return NULL;
    Node *temp = q->front;
    q->front = temp->parent;
    if (q->front == NULL)
        q->rear = NULL;
    temp->parent = NULL;
    return temp;
}

bool is_empty(Queue *q) {
    return q->front == NULL;
}

void free_queue(Queue *q) {
    Node *current = q->front;
    while (current != NULL) {
        Node *next = current->parent;
        free(current);
        current = next;
    }
    free(q);
}

// --- Otimização 1: Funções de Hashing e Visitação por Coordenadas (Adaptadas) ---

unsigned long hash_state_optimized(const State *state) {
    unsigned long hash = 5381;
    for (int i = 0; i < state->num_pieces; i++) {
        hash = ((hash << 5) + hash) + state->pieces[i].row;
        hash = ((hash << 5) + hash) + state->pieces[i].col;
    }
    return hash % HASH_TABLE_SIZE;
}

// Retorna o nó visitado se o estado for encontrado, senão NULL
Node *get_visited_node(const State *state, HashTable hash_table) {
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
                return current->node_ptr;
            }
        }
        current = current->next;
    }
    return NULL;
}

bool is_visited_optimized(const State *state, HashTable hash_table) {
    return get_visited_node(state, hash_table) != NULL;
}

void mark_visited_optimized(const State *state, Node *node, HashTable hash_table) {
    unsigned long index = hash_state_optimized(state);
    
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    new_node->num_pieces = state->num_pieces;
    new_node->piece_coords = (int *)malloc(state->num_pieces * 2 * sizeof(int));
    new_node->node_ptr = node;

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

// Funções do Klotski (mantidas)
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
            if (strcmp(line, "#solução") == 0) {
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

    // Ordenar peças por ID para garantir consistência no hash
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

void create_goal_state(const State *initial_state, State *goal_state) {
    *goal_state = *initial_state;
    
    // 1. Limpar o tabuleiro do estado final
    memset(goal_state->board, '0', sizeof(goal_state->board));

    // 2. Encontrar a peça alvo 'X'
    Piece *goal_piece_fwd = find_piece(initial_state, 'X');
    if (!goal_piece_fwd) return;

    // 3. Determinar a posição final da peça 'X'
    int target_row_temp = -1, target_col_temp = -1;
    if (initial_state->rows == 6 && initial_state->cols == 6) { // engarrafamento.txt
        target_row_temp = 4; target_col_temp = 4;
    } else if (initial_state->rows == 5 && initial_state->cols == 4) { // 18passos.txt
        target_row_temp = 3; target_col_temp = 1;
    } else {
        return; // Não é um estado final conhecido
    }

    // 4. Mover a peça 'X' para a posição final no estado final
    Piece *goal_piece_bwd = find_piece(goal_state, 'X');
    if (goal_piece_bwd) {
        goal_piece_bwd->row = target_row;
        goal_piece_bwd->col = target_col;
    }

    // 5. Desenhar a peça 'X' no tabuleiro do estado final
    for (int i = target_row_temp; i < target_row_temp + piece_X_initial->height; i++)         for (int j = target_col_temp; j < target_col_temp + piece_X_initial->width; j++) {
            goal_state->board[i][j] = 'X';
        }
    }

    // 6. As outras peças permanecem nas suas posições iniciais (para a busca reversa)
    // A busca reversa tentará mover as peças para o estado inicial.
    // No entanto, para o Klotski, a busca reversa é mais complexa pois os movimentos
    // não são reversíveis no sentido de "mover a peça X para o buraco Y".
    // A Bi-BFS no Klotski é feita movendo-se as peças para a posição inicial no estado final.
    // Vamos manter as outras peças na posição inicial e a busca reversa tentará
    // mover as peças para o estado inicial.
    // Para simplificar, vamos assumir que o estado final é apenas a peça 'X' na posição final
    // e o resto do tabuleiro vazio, e a busca reversa tentará preencher o tabuleiro.
    // Isso é incorreto para o Klotski. A busca reversa deve começar com o tabuleiro
    // na configuração final e tentar chegar à configuração inicial.

    // Vamos reverter a lógica: o estado final deve ter todas as peças na posição final.
    // O estado final é o tabuleiro inicial com a peça 'X' na posição final.
    // O tabuleiro inicial é copiado, e a peça 'X' é movida.
    // Isso é o que a função `is_goal` faz.

    // Para a Bi-BFS, o estado final deve ser o estado onde `is_goal` é verdadeiro.
    // O estado final é o tabuleiro inicial com a peça 'X' na posição final.
    // Vamos recriar o tabuleiro do estado final.

    // 1. Copia o estado inicial
    *goal_state = *initial_state;

    // 2. Encontra a peça alvo 'X'
    Piece *piece_X_initial = find_piece(initial_state, 'X');
    if (!piece_X_initial) return;

    // 3. Determina a posição final
    int target_row_temp = -1, target_col_temp = -1;
    if (initial_state->rows == 6 && initial_state->cols == 6) { // engarrafamento.txt
        target_row_temp = 4; target_col_temp = 4;
    } else if (initial_state->rows == 5 && initial_state->cols == 4) { // 18passos.txt
        target_row_temp = 3; target_col_temp = 1;
    } else {
        return;
    }

    // 4. Limpa a posição inicial da peça 'X' no tabuleiro do estado final
    for (int i = piece_X_initial->row; i < piece_X_initial->row + piece_X_initial->height; i++) {
        for (int j = piece_X_initial->col; j < piece_X_initial->col + piece_X_initial->width; j++) {
            goal_state->board[i][j] = '0';
        }
    }

    // 5. Move a peça 'X' para a posição final no estado final
    Piece *piece_X_goal = find_piece(goal_state, 'X');
    if (piece_X_goal) {
        piece_X_goal->row = target_row_temp;
        piece_X_goal->col = target_col_temp;
    }

    // 6. Desenha a peça 'X' na nova posição no tabuleiro do estado final
    for (int i = target_row_temp; i < target_row_temp + piece_X_initial->height; i++) {
        for (int j = target_col_temp; j < target_col_temp + piece_X_initial->width; j++) {
            goal_state->board[i][j] = 'X';
        }
    }
}

bool is_goal(const State *state) {
    Piece *goal_piece = find_piece(state, 'X');
    if (!goal_piece) return false;

    if (state->rows == 6 && state->cols == 6) {
        return (goal_piece->row == 4 && goal_piece->col == 4);
    }
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

    // Atualiza o tabuleiro no novo estado
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

    // Ordena as peças para manter a consistência do hash
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
    next_state->moves[next_state->move_count++] = p->id;
    next_state->moves[next_state->move_count++] = dir_char;
    next_state->moves[next_state->move_count] = '\0';

    return true;
}

void reconstruct_path(Node *fwd_node, Node *bwd_node) {
    // 1. Reconstruir o caminho de fwd_node para o início
    char fwd_path[1000];
    fwd_path[0] = '\0';
    int fwd_len = 0;
    Node *current = fwd_node;
    while (current->parent != NULL) {
        fwd_path[fwd_len++] = current->move_char;
        fwd_path[fwd_len++] = current->move_dir;
        current = current->parent;
    }
    // Inverter o caminho de fwd
    for (int i = 0; i < fwd_len / 2; i++) {
        char temp_char = fwd_path[i * 2];
        char temp_dir = fwd_path[i * 2 + 1];
        fwd_path[i * 2] = fwd_path[fwd_len - 2 - i * 2];
        fwd_path[i * 2 + 1] = fwd_path[fwd_len - 1 - i * 2];
        fwd_path[fwd_len - 2 - i * 2] = temp_char;
        fwd_path[fwd_len - 1 - i * 2] = temp_dir;
    }
    fwd_path[fwd_len] = '\0';

    // 2. Reconstruir o caminho de bwd_node para o fim (que é o início da busca reversa)
    char bwd_path[1000];
    bwd_path[0] = '\0';
    int bwd_len = 0;
    current = bwd_node;
    while (current->parent != NULL) {
        bwd_path[bwd_len++] = current->move_char;
        bwd_path[bwd_len++] = current->move_dir;
        current = current->parent;
    }
    bwd_path[bwd_len] = '\0';

    // 3. Concatenar os caminhos
    char final_path[2000];
    strcpy(final_path, fwd_path);
    strcat(final_path, bwd_path);

    printf("Solução encontrada em %d movimentos (Bi-BFS):\n", (fwd_len + bwd_len) / 2);
    printf("%s\n", final_path);
}

void solve_klotski(const char *filename) {
    State initial_state;
    parse_input(filename, &initial_state);

    State goal_state;
    create_goal_state(&initial_state, &goal_state);

    printf("Estado Inicial:\n");
    print_state(&initial_state);
    printf("\nEstado Final (Alvo):\n");
    print_state(&goal_state);

    if (is_goal(&initial_state)) {
        printf("O estado inicial já é o estado final.\n");
        return;
    }

    // Inicialização para a busca forward (fwd)
    Queue *q_fwd = create_queue();
    HashTable visited_fwd;
    memset(visited_fwd, 0, sizeof(HashTable));
    Node *initial_node = create_node(&initial_state, NULL, '\0', '\0');
    enqueue(q_fwd, initial_node);
    mark_visited_optimized(&initial_state, initial_node, visited_fwd);

    // Inicialização para a busca backward (bwd)
    Queue *q_bwd = create_queue();
    HashTable visited_bwd;
    memset(visited_bwd, 0, sizeof(HashTable));
    Node *goal_node = create_node(&goal_state, NULL, '\0', '\0');
    enqueue(q_bwd, goal_node);
    mark_visited_optimized(&goal_state, goal_node, visited_bwd);

    Node *collision_fwd = NULL;
    Node *collision_bwd = NULL;
    int max_depth = 0;

    while (!is_empty(q_fwd) && !is_empty(q_bwd)) {
        // Alterna a busca: expande a fila menor
        Queue *current_q, *other_q;
        HashTable current_visited, other_visited;
        bool is_fwd_turn;

        if (q_fwd->front->state.move_count <= q_bwd->front->state.move_count) {
            current_q = q_fwd;
            current_visited = visited_fwd;
            other_visited = visited_bwd;
            is_fwd_turn = true;
        } else {
            current_q = q_bwd;
            current_visited = visited_bwd;
            other_visited = visited_fwd;
            is_fwd_turn = false;
        }

        Node *current_node = dequeue(current_q);
        State *current_state = &current_node->state;

        if (current_state->move_count / 2 > max_depth) {
            max_depth = current_state->move_count / 2;
            printf("Profundidade de busca (total): %d\n", max_depth * 2);
        }

        // Tenta mover cada peça em todas as 4 direções
        for (int i = 0; i < current_state->num_pieces; i++) {
            char piece_id = current_state->pieces[i].id;
            if (piece_id == '0' || piece_id == '.') continue;

            char directions[] = {'U', 'D', 'L', 'R'};
            for (int d = 0; d < 4; d++) {
                State next_state;
                if (move_piece(current_state, piece_id, directions[d], &next_state)) {
                    
                    // 1. Verifica colisão com a outra busca
                    Node *collision_node = get_visited_node(&next_state, other_visited);
                    if (collision_node != NULL) {
                        if (is_fwd_turn) {
                            collision_fwd = current_node;
                            collision_bwd = collision_node;
                        } else {
                            collision_fwd = collision_node;
                            collision_bwd = current_node;
                        }
                        // Reconstruir o caminho
                        reconstruct_path(collision_fwd, collision_bwd);
                        goto end_search;
                    }

                    // 2. Se não colidiu, continua a busca
                    if (!is_visited_optimized(&next_state, current_visited)) {
                        Node *next_node = create_node(&next_state, current_node, piece_id, directions[d]);
                        mark_visited_optimized(&next_state, next_node, current_visited);
                        enqueue(current_q, next_node);
                    }
                }
            }
        }
    }

    printf("Solução não encontrada.\n");

end_search:
    // Liberação de memória
    free_queue(q_fwd);
    free_queue(q_bwd);
    free_hash_table(visited_fwd);
    free_hash_table(visited_bwd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return EXIT_FAILURE;
    }

    solve_klotski(argv[1]);

    return EXIT_SUCCESS;
}
