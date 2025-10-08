/*
 * 
 * Copyright 2025 Martinson <martinson@martinson-VPCEB17FB>
 * 
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define KEY_SIZE 29
#define MAX_PHRASE_SIZE 256


// Protótipos das funções
int get_char_value(char c);
int is_valid_char(char c);
void generate_key(const char* phrase, char* key);
void encrypt_phrase(const char* phrase, const char* key, char* encrypted_phrase);

/* -------------------------------------------------------------------------- */
/*                            FUNÇÕES AUXILIARES                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Retorna o valor numérico de um caractere válido.
 * Retorna 0 se o caractere for inválido.
 */
int get_char_value(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 1;
    } else if (c == ',') {
        return 27;
    } else if (c == '.') {
        return 28;
    } else if (c == ' ') {
        return 29;
    }
    return 0; // Caractere inválido
}

/**
 * @brief Verifica se um caractere é válido (letra, ',', '.', ' ').
 */
int is_valid_char(char c) {
    return (isalpha(c) || c == ',' || c == '.' || c == ' ');
}


/* -------------------------------------------------------------------------- */
/*                  PARTE 1: FUNÇÕES DE GERAÇÃO DA CHAVE                        */
/* -------------------------------------------------------------------------- */

/**
 * @brief a) Converte para maiúsculas e b) remove caracteres inválidos e duplicados.
 * Esta função combina os passos (a) e (b) do exercício.
 */
void filter_and_remove_duplicates(const char* input, char* output) {
    int k = 0;
    int seen[256] = {0}; // Tabela para rastrear caracteres já vistos

    for (int i = 0; input[i] != '\0'; i++) {
        char current_char = toupper(input[i]);

        if (is_valid_char(current_char) && !seen[(unsigned char)current_char]) {
            output[k++] = current_char;
            seen[(unsigned char)current_char] = 1; // Marca como visto
        }
    }
    output[k] = '\0'; // Finaliza a string de saída
}

/**
 * @brief c) Completa a chave com os caracteres que faltam.
 */
void complete_key(char* key) {
    const char ALL_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ,. ";
    int len = strlen(key);

    for (int i = 0; i < KEY_SIZE; i++) {
        char char_to_check = ALL_CHARS[i];
        // strchr retorna NULL se o caractere não for encontrado na string 'key'
        if (strchr(key, char_to_check) == NULL) {
            key[len++] = char_to_check;
        }
    }
    key[len] = '\0'; // Garante que a chave termine corretamente
}

/**
 * @brief Função principal para gerar a chave, orquestrando as etapas.
 */
void generate_key(const char* phrase, char* key) {
    // Passos a) e b): Converte, filtra e remove duplicatas
    filter_and_remove_duplicates(phrase, key);
    
    // Passo c): Completa a chave com os caracteres faltantes
    complete_key(key);
}


/* -------------------------------------------------------------------------- */
/*                       PARTE 2: FUNÇÃO DE CRIPTOGRAFIA                      */
/* -------------------------------------------------------------------------- */

/**
 * @brief Criptografa uma frase usando a chave fornecida.
 */
void encrypt_phrase(const char* phrase, const char* key, char* encrypted_phrase) {
    int key_index = 0;
    int output_index = 0;

    for (int i = 0; phrase[i] != '\0'; i++) {
        char current_char = toupper(phrase[i]);
        int value = get_char_value(current_char);

        if (value > 0) { // Se o caractere for válido para criptografia
            // Move o índice na chave de forma circular
            key_index = (key_index + value) % KEY_SIZE;
            encrypted_phrase[output_index++] = key[key_index];
        } else {
            // Mantém caracteres inválidos como estão na saída
            encrypted_phrase[output_index++] = phrase[i];
        }
    }
    encrypted_phrase[output_index] = '\0'; // Finaliza a string criptografada
}

/* -------------------------------------------------------------------------- */
/*                      FUNÇÕES AUXILIARES PARA DESCRIPTOGRAFIA               */
/* -------------------------------------------------------------------------- */

/**
 * @brief Retorna a posição (índice) de um caractere na chave.
 * Retorna -1 se não encontrar.
 */
int get_char_position(const char* key, char c) {
    for (int i = 0; i < KEY_SIZE; i++) {
        if (key[i] == c) {
            return i;
        }
    }
    return -1; // Não encontrado
}

/**
 * @brief Retorna o caractere correspondente a um valor numérico.
 * Retorna '\0' se o valor for inválido.
 */
char get_char_from_value(int value) {
    if (value >= 1 && value <= 26) {
        return 'A' + value - 1;
    } else if (value == 27) {
        return ',';
    } else if (value == 28) {
        return '.';
    } else if (value == 29) {
        return ' ';
    }
    return '\0'; // Valor inválido
}


/* -------------------------------------------------------------------------- */
/*                       PARTE 4: FUNÇÃO DE DESCRIPTOGRAFIA                   */
/* -------------------------------------------------------------------------- */

/**
 * @brief Descriptografa uma frase usando a chave fornecida.
 */
void decrypt_phrase(const char* encrypted_phrase, const char* key, char* decrypted_phrase) {
    int prev_key_index = 0;
    int output_index = 0;

    for (int i = 0; encrypted_phrase[i] != '\0'; i++) {
        char current_char = encrypted_phrase[i];
        int current_key_index = get_char_position(key, current_char);

        if (current_key_index != -1) { // Se o caractere está na chave
            // Calcula a distância circular para trás
            int original_value = (current_key_index - prev_key_index + KEY_SIZE) % KEY_SIZE;
            
            // O módulo % KEY_SIZE resulta em 0 para múltiplos de KEY_SIZE.
            // O valor 29 (espaço) se tornaria 0. Precisamos corrigir isso.
            if (original_value == 0) {
                original_value = KEY_SIZE;
            }

            char original_char = get_char_from_value(original_value);
            if (original_char != '\0') {
                decrypted_phrase[output_index++] = original_char;
            }

            // Atualiza o índice anterior para a próxima iteração
            prev_key_index = current_key_index;
        } else { {
            // Mantém caracteres que não fazem parte da criptografia (ex: quebra de linha)
            decrypted_phrase[output_index++] = current_char;
        }
    }
    decrypted_phrase[output_index] = '\0';
}


/* -------------------------------------------------------------------------- */
/*                         PARTE 5: FUNÇÃO PRINCIPAL                          */
/* -------------------------------------------------------------------------- */

int main() {
    char phrase_for_key[MAX_PHRASE_SIZE];
    char phrase_to_encrypt[MAX_PHRASE_SIZE];
    char key[KEY_SIZE + 1]; // +1 para o caractere nulo '\0'
    char encrypted_result[MAX_PHRASE_SIZE];
    char decrypted_result[MAX_PHRASE_SIZE];

    // 1. Ler a frase para gerar a chave
    printf("Digite a frase para gerar a chave: ");
    fgets(phrase_for_key, sizeof(phrase_for_key), stdin);
    // Remove a quebra de linha que o fgets adiciona
    phrase_for_key[strcspn(phrase_for_key, "\n")] = 0;

    // 2. Gerar a chave válida
    generate_key(phrase_for_key, key);
    printf("\nChave gerada: \"%s\"\n", key);

    // 3. Ler a frase para criptografar
    printf("\nDigite a frase a ser criptografada: ");
    fgets(phrase_to_encrypt, sizeof(phrase_to_encrypt), stdin);
    phrase_to_encrypt[strcspn(phrase_to_encrypt, "\n")] = 0;

    // 4. Criptografar a segunda frase usando a chave criada
    encrypt_phrase(phrase_to_encrypt, key, encrypted_result);

    // 5. Exibir o resultado
    printf("\nFrase criptografada: \"%s\"\n", encrypted_result);

    // 6. Descriptografar a mensagem para verificar
    decrypt_phrase(encrypted_result, key, decrypted_result);
    printf("\nFrase descriptografada: \"%s\"\n", decrypted_result);

    return 0;
}

