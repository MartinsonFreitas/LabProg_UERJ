#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// Definições do tabuleiro
#define MAX_ROWS 10
#define MAX_COLS 10
#define MAX_PIECES 26 // 'a' a 'z'
#define MAX_MOVES 10000
#define HASH_TABLE_SIZE 1000003 // Número primo grande

// Estrutura para representar uma peça (mantida para parsing e dimensões)
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
    unsigned long long state_key; // Representação compacta do estado (long long)
    int rows;
    int cols;
    Piece pieces[MAX_PIECES]; // Posições e dimensões das peças
    int num_pieces;
} State;

// Estrutura para o conjunto de estados visitados (Hash Set)
// Armazena apenas a chave compacta
typedef struct HashNode {
    unsigned long long state_key;
    struct HashNode *next;
} HashNode;

typedef HashNode *HashTable[HASH_TABLE_SIZE];

// Variáveis globais para a solução
char final_path[MAX_MOVES * 2];
int final_path_len = 0;
bool solution_found = false;
int min_cost = MAX_MOVES + 1;

// Protótipos
void parse_input(const char *filename, State *initial_state);
void initialize_state(State *state);
void print_state(const State *state);
unsigned long hash_state_compact(unsigned long long state_key);
bool is_visited(unsigned long long state_key, HashTable hash_table);
void add_visited(unsigned long long state_key, HashTable hash_table);
void remove_visited(unsigned long long state_key, HashTable hash_table);
void free_hash_table(HashTable hash_table);
bool is_goal(const State *state);
Piece *find_piece(State *state, char id);
bool move_piece(const State *current_state, char piece_id, char direction, State *next_state);
int calculate_heuristic(const State *state);
int calculate_blocking_heuristic(const State *state);
void update_state_key(State *state);
int search_ida_star(State *current_state, int g, int limit, HashTable visited);
void solve_klotski_ida_star(const char *filename);

// --- Funções de Hashing e Visitação (Adaptadas para Chave Compacta e Backtracking Removal) ---

// Gera a chave compacta do estado a partir das coordenadas das peças
void update_state_key(State *state) {
    unsigned long long key = 0;
    for (int i = 0; i < state->num_pieces; i++) {
        // Codifica (row, col) em 6 bits (3 bits para row, 3 bits para col)
        // Assume que MAX_ROWS e MAX_COLS são <= 8 (3 bits)
        unsigned long long row_bits = (unsigned long long)state->pieces[i].row & 0x7;
        unsigned long long col_bits = (unsigned long long)state->pieces[i].col & 0x7;
        
        // Combina row e col em 6 bits
        unsigned long long pos_bits = (row_bits << 3) | col_bits;
        
        // Desloca para a posição correta na chave (6 bits por peça)
        key |= (pos_bits << (i * 6));
    }
    state->state_key = key;
}

unsigned long hash_state_compact(unsigned long long state_key) {
    // Função de hash simples para long long
    return (unsigned long)(state_key % HASH_TABLE_SIZE);
}

bool is_visited(unsigned long long state_key, HashTable hash_table) {
    unsigned long index = hash_state_compact(state_key);
    HashNode *current = hash_table[index];

    while (current != NULL) {
        if (current->state_key == state_key) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void add_visited(unsigned long long state_key, HashTable hash_table) {
    unsigned long index = hash_state_compact(state_key);
    
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    new_node->state_key = state_key;

    new_node->next = hash_table[index];
    hash_table[index] = new_node;
}

void remove_visited(unsigned long long state_key, HashTable hash_table) {
    unsigned long index = hash_state_compact(state_key);
    HashNode *current = hash_table[index];
    HashNode *prev = NULL;

    while (current != NULL) {
        if (current->state_key == state_key) {
            if (prev == NULL) {
                hash_table[index] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
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

// --- Funções do Klotski (Reutilizadas e Adaptadas) ---

void initialize_state(State *state) {
    state->num_pieces = 0;
    state->rows = 0;
    state->cols = 0;
    state->state_key = 0;
    memset(state->pieces, 0, sizeof(state->pieces));
}

Piece *find_piece(State *state, char id) {
    for (int i = 0; i < state->num_pieces; i++) {
        if (state->pieces[i].id == id) {
            return &state->pieces[i];
        }
    }
    return NULL;
}

// Função auxiliar para reconstruir o tabuleiro a partir da chave compacta (para visualização e validação de movimento)
void get_board_from_state(const State *state, char board[MAX_ROWS][MAX_COLS]) {
    memset(board, '0', sizeof(char) * MAX_ROWS * MAX_COLS);
    for (int i = 0; i < state->num_pieces; i++) {
        const Piece *p = &state->pieces[i];
        for (int r = p->row; r < p->row + p->height; r++) {
            for (int c = p->col; c < p->col + p->width; c++) {
                if (r >= 0 && r < state->rows && c >= 0 && c < state->cols) {
                    board[r][c] = p->id;
                }
            }
        }
    }
}

void parse_input(const char *filename, State *initial_state) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo de entrada");
        exit(EXIT_FAILURE);
    }

    initialize_state(initial_state);
    char board_temp[MAX_ROWS][MAX_COLS];
    memset(board_temp, '0', sizeof(board_temp));
    
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
                board_temp[row][col] = id;

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
            if (board_temp[r][p->col] == p->id) max_row = r; else break;
        }
        p->height = max_row - p->row + 1;

        int max_col = p->col;
        for (int c = p->col; c < initial_state->cols; c++) {
            if (board_temp[p->row][c] == p->id) max_col = c; else break;
        }
        p->width = max_col - p->col + 1;
    }
    
    // Gerar a chave compacta inicial
    update_state_key(initial_state);
}

void print_state(const State *state) {
    char board[MAX_ROWS][MAX_COLS];
    get_board_from_state(state, board);
    for (int i = 0; i < state->rows; i++) {
        for (int j = 0; j < state->cols; j++) {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
}

bool is_goal(const State *state) {
    // É necessário um cast para State* para usar find_piece, pois ele modifica o estado
    // (embora não devesse, mas a estrutura Piece é modificada internamente)
    Piece *goal_piece = find_piece((State *)state, 'X');
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

    char board[MAX_ROWS][MAX_COLS];
    get_board_from_state(state, board);

    for (int r = new_row; r < new_row + p->height; r++) {
        for (int c = new_col; c < new_col + p->width; c++) {
            char cell = board[r][c];
            if (cell != '0' && cell != '.' && cell != p->id) {
                return false;
            }
        }
    }
    return true;
}

bool move_piece(const State *current_state, char piece_id, char direction, State *next_state) {
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

    p->row += dr;
    p->col += dc;

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
    
    // Atualiza a chave compacta
    update_state_key(next_state);

    return true;
}

// Heurística de Bloqueio: Conta o número de peças que bloqueiam o caminho da peça 'X' até a saída.
int calculate_blocking_heuristic(const State *state) {
    Piece *goal_piece = find_piece((State *)state, 'X');
    if (!goal_piece) return 0;

    int target_row = -1;
    if (state->rows == 6 && state->cols == 6) {
        target_row = 4;
    } else if (state->rows == 5 && state->cols == 4) {
        target_row = 3;
    } else {
        return 0;
    }

    // A peça 'X' é 2x2. O caminho crítico é a área abaixo de 'X' até a linha de saída.
    // Contamos o número de peças que estão na área de colunas de 'X' e abaixo de 'X'.
    int blocking_count = 0;
    
    // A área crítica começa na linha abaixo da peça 'X'
    int start_row = goal_piece->row + goal_piece->height; 
    
    // A área crítica termina na linha de saída (target_row + goal_piece->height - 1)
    int end_row = target_row + goal_piece->height; 

    for (int r = start_row; r < end_row; r++) {
        for (int c = goal_piece->col; c < goal_piece->col + goal_piece->width; c++) {
            // Verifica se a célula (r, c) está ocupada por alguma peça diferente de 'X' ou '0'
            char board[MAX_ROWS][MAX_COLS];
            get_board_from_state(state, board);
            
            if (r >= 0 && r < state->rows && c >= 0 && c < state->cols) {
                char cell_id = board[r][c];
                if (cell_id != '0' && cell_id != '.' && cell_id != goal_piece->id) {
                    // Se a célula está ocupada, precisamos encontrar a peça que a ocupa
                    // e garantir que ela seja contada apenas uma vez.
                    // Uma forma simples é contar o número de peças que têm sua
                    // coordenada superior-esquerda (r, c) na área crítica.
                    
                    // Como a heurística de bloqueio é mais complexa, vamos simplificar:
                    // Se a peça 'X' não está na linha de saída, a heurística de bloqueio é
                    // o número de peças que estão abaixo da peça 'X' e precisam ser movidas.
                    
                    // Simplificação: Contar o número de peças distintas que estão abaixo da peça 'X'
                    // e que estão nas colunas de 'X'.
                    
                    for (int i = 0; i < state->num_pieces; i++) {
                        const Piece *p = &state->pieces[i];
                        if (p->id == 'X' || p->id == '0' || p->id == '.') continue;
                        
                        // Verifica se a peça 'p' está na área crítica
                        bool in_x_cols = (p->col < goal_piece->col + goal_piece->width) && 
                                         (p->col + p->width > goal_piece->col);
                        
                        bool below_x = (p->row >= goal_piece->row + goal_piece->height);
                        
                        if (in_x_cols && below_x) {
                            // Contamos apenas se a peça 'p' está na linha de 'X' ou abaixo
                            // e está nas colunas de 'X'.
                            // Para evitar contagem duplicada, contamos apenas a peça que
                            // tem sua coordenada superior-esquerda na área crítica.
                            if (p->row == r && p->col == c) {
                                blocking_count++;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Heurística de Bloqueio: Se a peça 'X' não está na linha de saída,
    // a heurística é o número de peças que estão abaixo da peça 'X' e precisam ser movidas.
    // A heurística mais comum é: 1 + (número de peças bloqueadoras distintas).
    
    // Vamos usar a heurística de bloqueio mais robusta:
    // 1. Se a peça 'X' não está na linha de saída, a heurística é 1 + (número de peças bloqueadoras).
    // 2. Se a peça 'X' está na linha de saída, a heurística é 0.
    
    if (goal_piece->row < target_row) {
        // A peça 'X' ainda não está na linha de saída.
        // Contamos o número de peças que estão abaixo da peça 'X' e que estão nas colunas de 'X'.
        
        int distinct_blocking_pieces = 0;
        bool is_piece_counted[MAX_PIECES] = {false};
        
        for (int i = 0; i < state->num_pieces; i++) {
            const Piece *p = &state->pieces[i];
            if (p->id == 'X' || p->id == '0' || p->id == '.') continue;
            
            // Verifica se a peça 'p' está nas colunas de 'X'
            bool in_x_cols = (p->col < goal_piece->col + goal_piece->width) && 
                             (p->col + p->width > goal_piece->col);
            
            // Verifica se a peça 'p' está abaixo da peça 'X'
            bool below_x = (p->row >= goal_piece->row + goal_piece->height);
            
            if (in_x_cols && below_x) {
                // Contamos apenas se a peça não foi contada
                if (!is_piece_counted[i]) {
                    distinct_blocking_pieces++;
                    is_piece_counted[i] = true;
                }
            }
        }
        
        // A heurística de bloqueio é 1 + (número de peças bloqueadoras)
        return 1 + distinct_blocking_pieces;
    }
    
    return 0;
}

int calculate_heuristic(const State *state) {
    // É necessário um cast para State* para usar find_piece
    Piece *goal_piece = find_piece((State *)state, 'X');
    if (!goal_piece) return 10000;

    int target_row = -1, target_col = -1;
    if (state->rows == 6 && state->cols == 6) {
        target_row = 4; target_col = 4;
    } else if (state->rows == 5 && state->cols == 4) {
        target_row = 3; target_col = 1;
    } else {
        return 10000;
    }

    // Heurística de Manhattan (h1)
    int dr = abs(goal_piece->row - target_row);
    int dc = abs(goal_piece->col - target_col);
    int h1 = dr + dc;
    
    // Heurística de Bloqueio (h2)
    int h2 = calculate_blocking_heuristic(state);

    // A heurística combinada é max(h1, h2) para garantir a admissibilidade
    return (h1 > h2) ? h1 : h2;
}


// --- Algoritmo IDA* Recursivo com Backtracking Removal e Estado Compacto ---

// Função recursiva de busca com limite de custo f
int search_ida_star(State *current_state, int g, int limit, HashTable visited) {
    int h = calculate_heuristic(current_state);
    int f = g + h;

    if (f > limit) {
        return f; // Retorna o novo limite mínimo
    }

    if (is_goal(current_state)) {
        solution_found = true;
        return g; // Retorna o custo real (g)
    }

    // Poda: Se o estado já está no caminho atual, é um ciclo.
    if (is_visited(current_state->state_key, visited)) {
        return limit + 1; // Poda o ciclo
    }
    
    // Adiciona o estado ao Hash Set (apenas para o caminho atual)
    add_visited(current_state->state_key, visited);

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
                    // Remove o estado atual do Hash Set antes de retornar
                    remove_visited(current_state->state_key, visited);
                    return result;
                }

                // Backtracking: Remove o movimento do caminho atual
                final_path_len -= 2;
                
                if (result < min_f) {
                    min_f = result;
                }
            }
        }
    }
    
    // Backtracking Removal: Remove o estado do Hash Set ao retornar
    remove_visited(current_state->state_key, visited);
    
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
    int max_limit = MAX_MOVES;
    
    HashTable visited = {0};
    
    printf("Iniciando busca IDA* Recursiva com Estado Compacto e Backtracking Removal...\n");

    while (limit <= max_limit && !solution_found) {
        printf("Iteração com limite F = %d\n", limit);
        
        if (limit % 1000 == 0) {
            printf("\n--- ESTADO INICIAL NA ITERAÇÃO F = %d ---\n", limit);
            print_state(&initial_state);
            printf("------------------------------------------\n");
        }
        
        // Limpa o Hash Set a cada iteração
        free_hash_table(visited);
        memset(visited, 0, sizeof(visited));
        
        final_path_len = 0;
        final_path[0] = '\0';
        
        // Adiciona o estado inicial ao Hash Set para a primeira chamada
        add_visited(initial_state.state_key, visited);
        
        int next_limit = search_ida_star(&initial_state, 0, limit, visited);
        
        // Remove o estado inicial do Hash Set após a iteração
        remove_visited(initial_state.state_key, visited);
        
        if (solution_found) {
            printf("Solução encontrada em %d movimentos (IDA* Final):\n", limit);
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
