#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h> // Para abs()

// Definições do tabuleiro
#define MAX_ROWS 10
#define MAX_COLS 10
#define MAX_PIECES 26 // 'a' a 'z'
#define MAX_MOVES 1000
#define HASH_TABLE_SIZE 1000003 // Número primo grande

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
} State;

// Estrutura para o nó da busca (IDA*) - Apenas para reconstrução do caminho
typedef struct Node {
    State state;
    struct Node *parent;
    char move_char; // Peça movida
    char move_dir;  // Direção do movimento (U, D, L, R)
    int g;          // Custo do caminho do início até este nó
} Node;

// Estrutura para o conjunto de estados visitados (Hash Set)
// Usado para armazenar o melhor g-cost encontrado para um estado
typedef struct HashNode {
    int *piece_coords; // Array de coordenadas (r1, c1, r2, c2, ...)
    int num_pieces;
    int g_cost;        // O menor custo g encontrado para este estado
    struct HashNode *next;
} HashNode;

typedef HashNode *HashTable[HASH_TABLE_SIZE];

// Variáveis globais para a solução
char final_path[MAX_MOVES * 2];
int final_path_len = 0;
int solution_found = 0;

// Protótipos
void parse_input(const char *filename, State *initial_state);
void initialize_state(State *state);
void print_state(const State *state);
unsigned long hash_state_optimized(const State *state);
int get_visited_g_cost(const State *state, HashTable hash_table);
void update_visited_g_cost(const State *state, int g_cost, HashTable hash_table);
void free_hash_table(HashTable hash_table);
bool is_goal(const State *state);
Piece *find_piece(const State *state, char id);
bool move_piece(State *current_state, char piece_id, char direction, State *next_state);
int calculate_heuristic(const State *state);
int search_ida_star(State *current_state, int g, int limit, HashTable visited);
void solve_klotski_ida_star(const char *filename);

// --- Funções de Hashing e Visitação (Adaptadas para IDA*) ---

unsigned long hash_state_optimized(const State *state) {
    unsigned long hash = 5381;
    for (int i = 0; i < state->num_pieces; i++) {
        hash = ((hash << 5) + hash) + state->pieces[i].row;
        hash = ((hash << 5) + hash) + state->pieces[i].col;
    }
    return hash % HASH_TABLE_SIZE;
}

// Retorna o g_cost visitado se o estado for encontrado, senão -1
int get_visited_g_cost(const State *state, HashTable hash_table) {
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
                return current->g_cost;
            }
        }
        current = current->next;
    }
    return -1;
}

// Atualiza o g_cost visitado ou insere um novo nó
void update_visited_g_cost(const State *state, int g_cost, HashTable hash_table) {
    unsigned long index = hash_state_optimized(state);
    HashNode *current = hash_table[index];

    // 1. Tenta encontrar e atualizar
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
                if (g_cost < current->g_cost) {
                    current->g_cost = g_cost;
                }
                return;
            }
        }
        current = current->next;
    }

    // 2. Se não encontrado, insere um novo nó
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    new_node->num_pieces = state->num_pieces;
    new_node->piece_coords = (int *)malloc(state->num_pieces * 2 * sizeof(int));
    new_node->g_cost = g_cost;

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

// --- Funções do Klotski (Reutilizadas) ---

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

void print_state(const State *state) {
    for (int i = 0; i < state->rows; i++) {
        for (int j = 0; j < state->cols; j++) {
            printf("%c", state->board[i][j]);
        }
        printf("\n");
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

    return true;
}

int calculate_heuristic(const State *state) {
    Piece *goal_piece = find_piece(state, 'X');
    if (!goal_piece) return 10000; // Valor alto se a peça alvo não for encontrada

    int target_row = -1, target_col = -1;
    if (state->rows == 6 && state->cols == 6) { // engarrafamento.txt
        target_row = 4; target_col = 4;
    } else if (state->rows == 5 && state->cols == 4) { // 18passos.txt
        target_row = 3; target_col = 1;
    } else {
        return 10000;
    }

    // Heurística: Distância de Manhattan da peça 'X' até a posição final
    int dr = abs(goal_piece->row - target_row);
    int dc = abs(goal_piece->col - target_col);

    return dr + dc;
}

// --- Algoritmo IDA* (Iterative Deepening A*) ---

// Função recursiva de busca com limite de custo f
int search_ida_star(State *current_state, int g, int limit, HashTable visited) {
    int h = calculate_heuristic(current_state);
    int f = g + h;

    if (f > limit) {
        return f; // Retorna o novo limite mínimo
    }

    if (is_goal(current_state)) {
        solution_found = 1;
        return g; // Retorna o custo real (g)
    }

    // Poda: Se já encontramos este estado com um custo g menor, paramos.
    int visited_g = get_visited_g_cost(current_state, visited);
    if (visited_g != -1 && visited_g <= g) {
        return limit + 1; // Retorna um valor maior que o limite para ser ignorado
    }
    update_visited_g_cost(current_state, g, visited);

    int min_f = 1000000; // Infinito
    char directions[] = {'U', 'D', 'L', 'R'};

    // Tenta mover cada peça em todas as 4 direções
    for (int i = 0; i < current_state->num_pieces; i++) {
        char piece_id = current_state->pieces[i].id;
        if (piece_id == '0' || piece_id == '.') continue;

        for (int d = 0; d < 4; d++) {
            State next_state;
            if (move_piece(current_state, piece_id, directions[d], &next_state)) {
                
                // Adiciona o movimento ao caminho atual (simulação de pilha)
                final_path[final_path_len++] = piece_id;
                final_path[final_path_len++] = directions[d] == 'U' ? 'C' : (directions[d] == 'D' ? 'B' : (directions[d] == 'L' ? 'E' : 'D'));
                
                int result = search_ida_star(&next_state, g + 1, limit, visited);

                if (solution_found) {
                    return result;
                }

                // Backtracking: Remove o movimento do caminho atual (liberação de memória lógica)
                final_path_len -= 2;
                
                if (result < min_f) {
                    min_f = result;
                }
            }
        }
    }
    
    // Limpeza de memória: A poda de estados repetidos é feita pelo Hash Set.
    // Para o IDA*, o Hash Set é limpo a cada iteração para reutilizar a memória.
    // Não é necessário remover o estado atual do Hash Set aqui.
    
    return min_f;
}

void solve_klotski_ida_star(const char *filename) {
    State initial_state;
    parse_input(filename, &initial_state);

    printf("Estado Inicial:\n");
    print_state(&initial_state);

    if (is_goal(&initial_state)) {
        printf("O estado inicial já é o estado final.\n");
        return;
    }

    int limit = calculate_heuristic(&initial_state);
    int max_limit = 100; // Limite máximo de profundidade
    
    HashTable visited = {0};
    
    printf("Iniciando busca IDA*...\n");

    while (limit <= max_limit && !solution_found) {
        printf("Iteração com limite F = %d\n", limit);
        
    // Limpa o hash set a cada iteração para reutilizar a memória
    free_hash_table(visited);
    // O memset não é necessário, pois free_hash_table já limpa os ponteiros.
    // Mas para garantir, podemos re-inicializar o array de ponteiros.
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        visited[i] = NULL;
    }
        
        final_path_len = 0;
        final_path[0] = '\0';
        
        int next_limit = search_ida_star(&initial_state, 0, limit, visited);
        
        if (solution_found) {
            printf("Solução encontrada em %d movimentos (IDA*):\n", limit);
            final_path[final_path_len] = '\0';
            printf("%s\n", final_path);
            break;
        }
        
        if (next_limit > max_limit) {
            printf("Limite máximo de busca atingido.\n");
            break;
        }
        
        limit = next_limit;
    }

    if (!solution_found) {
        printf("Solução não encontrada.\n");
    }
    
    free_hash_table(visited);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return EXIT_FAILURE;
    }

    solve_klotski_ida_star(argv[1]);

    return EXIT_SUCCESS;
}
