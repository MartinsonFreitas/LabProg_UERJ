/* * 
 * Copyright 2025 Martinson <martinson@martinson-VPCEB17FB>
 * */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h> // Necessário para exit()

#define KEY_SIZE 29
#define MAX_PHRASE_SIZE 256

// --- Protótipos de todas as funções ---
void limpar_buffer_entrada();
void exibir_menu();
int get_char_value(char c);
int is_valid_char(char c);
void generate_key(const char* phrase, char* key);
void to_uppercase_and_filter_invalid(const char* input, char* output);
void encrypt_phrase(const char* phrase, const char* key, char* encrypted_phrase);
void decrypt_phrase(const char* encrypted_phrase, const char* key, char* decrypted_phrase);

// --- Implementação das funções (sem alterações na lógica principal) ---
// (As funções de criptografia, descriptografia, etc., são as mesmas de antes)

int get_char_value(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A' + 1;
    if (c == ',') return 27;
    if (c == '.') return 28;
    if (c == ' ') return 29;
    return 0;
}

int is_valid_char(char c) {
    return (isalpha(c) || c == ',' || c == '.' || c == ' ');
}

void filter_and_remove_duplicates(const char* input, char* output) {
    int k = 0;
    int seen[256] = {0};
    for (int i = 0; input[i] != '\0'; i++) {
        char current_char = toupper(input[i]);
        if (is_valid_char(current_char) && !seen[(unsigned char)current_char]) {
            output[k++] = current_char;
            seen[(unsigned char)current_char] = 1;
        }
    }
    output[k] = '\0';
}

void complete_key(char* key) {
    const char ALL_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ,. ";
    int len = strlen(key);
    for (int i = 0; i < KEY_SIZE; i++) {
        if (strchr(key, ALL_CHARS[i]) == NULL) {
            key[len++] = ALL_CHARS[i];
        }
    }
    key[len] = '\0';
}

void generate_key(const char* phrase, char* key) {
    filter_and_remove_duplicates(phrase, key);
    complete_key(key);
}

void to_uppercase_and_filter_invalid(const char* input, char* output) {
    int k = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        char current_char = toupper(input[i]);
        if (is_valid_char(current_char)) {
            output[k++] = current_char;
        }
    }
    output[k] = '\0';
}

void encrypt_phrase(const char* phrase, const char* key, char* encrypted_phrase) {
    int key_index = 0;
    int output_index = 0;
    for (int i = 0; phrase[i] != '\0'; i++) {
        char current_char = toupper(phrase[i]);
        int value = get_char_value(current_char);
        if (value > 0) {
            key_index = (key_index + value) % KEY_SIZE;
            encrypted_phrase[output_index++] = key[key_index];
        } else {
            encrypted_phrase[output_index++] = phrase[i];
        }
    }
    encrypted_phrase[output_index] = '\0';
}

int get_char_position(const char* key, char c) {
    for (int i = 0; i < KEY_SIZE; i++) {
        if (key[i] == c) return i;
    }
    return -1;
}

char get_char_from_value(int value) {
    if (value >= 1 && value <= 26) return 'A' + value - 1;
    if (value == 27) return ',';
    if (value == 28) return '.';
    if (value == 29) return ' ';
    return '\0';
}

void decrypt_phrase(const char* encrypted_phrase, const char* key, char* decrypted_phrase) {
    int prev_key_index = 0;
    int output_index = 0;
    for (int i = 0; encrypted_phrase[i] != '\0'; i++) {
        char current_char = encrypted_phrase[i];
        int current_key_index = get_char_position(key, current_char);
        if (current_key_index != -1) {
            int original_value = (current_key_index - prev_key_index + KEY_SIZE) % KEY_SIZE;
            if (original_value == 0) original_value = KEY_SIZE;
            char original_char = get_char_from_value(original_value);
            if (original_char != '\0') {
                decrypted_phrase[output_index++] = original_char;
            }
            prev_key_index = current_key_index;
        } else {
            decrypted_phrase[output_index++] = current_char;
        }
    }
    decrypted_phrase[output_index] = '\0';
}

/* -------------------------------------------------------------------------- */
/*                         FUNÇÕES DO MENU INTERATIVO                         */
/* -------------------------------------------------------------------------- */

/**
 * @brief Limpa o buffer de entrada (stdin) para evitar problemas com leituras
 * de caracteres e strings após leituras de números.
 */
void limpar_buffer_entrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Exibe o menu de opções para o usuário.
 */
void exibir_menu() {
    printf("\n\n======== MENU ========\n");
    printf("1. Criptografar frase\n");
    printf("2. Descriptografar frase\n");
    printf("3. Sair\n");
    printf("======================\n");
    printf("Escolha uma opcao: ");
}

/* -------------------------------------------------------------------------- */
/*                 FUNÇÃO PRINCIPAL COM LÓGICA DE MENU                        */
/* -------------------------------------------------------------------------- */

int main() {
    int escolha;
    char frase_base[MAX_PHRASE_SIZE];
    char frase_processada[MAX_PHRASE_SIZE];
    char chave[KEY_SIZE + 1];
    char resultado[MAX_PHRASE_SIZE];

    while (1) { // Loop infinito do menu
        exibir_menu();
        scanf("%d", &escolha);
        limpar_buffer_entrada(); // Essencial após scanf

        switch (escolha) {
            case 1: // Criptografar
                printf("\n--- Criptografar ---\n");
                printf("Digite a frase para gerar a chave: ");
                fgets(frase_base, sizeof(frase_base), stdin);
                frase_base[strcspn(frase_base, "\n")] = 0;

                generate_key(frase_base, chave);
                printf("Chave gerada: \"%s\"\n", chave);

                printf("Digite a frase a ser criptografada: ");
                fgets(frase_base, sizeof(frase_base), stdin);
                frase_base[strcspn(frase_base, "\n")] = 0;
                
                to_uppercase_and_filter_invalid(frase_base, frase_processada);

                encrypt_phrase(frase_processada, chave, resultado);
                printf("Frase criptografada: \"%s\"\n", resultado);
                break;

            case 2: // Descriptografar
                printf("\n--- Descriptografar ---\n");
                printf("Digite a frase que gerou a chave: ");
                fgets(frase_base, sizeof(frase_base), stdin);
                frase_base[strcspn(frase_base, "\n")] = 0;

                generate_key(frase_base, chave);
                printf("Chave usada: \"%s\"\n", chave);

                printf("Digite a frase criptografada: ");
                fgets(frase_base, sizeof(frase_base), stdin);
                frase_base[strcspn(frase_base, "\n")] = 0;

                decrypt_phrase(frase_base, chave, resultado);
                printf("Frase descriptografada: \"%s\"\n", resultado);
                break;

            case 3: // Sair
                printf("\nSaindo do programa. Ate mais!\n");
                return 0; // Termina o programa

            default:
                printf("\nOpcao invalida! Por favor, escolha 1, 2 ou 3.\n");
                break;
        }
    }

    return 0;
}


