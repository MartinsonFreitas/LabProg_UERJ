/* * 
 * Copyright 2025 Martinson <martinson@martinson-VPCEB17FB>
 * */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define KEY_SIZE 29
#define MAX_PHRASE_SIZE 256

// --- Protótipos das funções (sem alterações) ---
int get_char_value(char c);
int is_valid_char(char c);
void generate_key(const char* phrase, char* key);
void encrypt_phrase(const char* phrase, const char* key, char* encrypted_phrase);
void decrypt_phrase(const char* encrypted_phrase, const char* key, char* decrypted_phrase);
void to_uppercase_and_filter_invalid(const char* input, char* output);

// --- Implementação das funções (sem alterações) ---

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

/* -------------------------------------------------------------------------- */
/*                 FUNÇÃO PRINCIPAL COM SAÍDA FORMATADA                       */
/* -------------------------------------------------------------------------- */

int main() {
    char original_phrase[MAX_PHRASE_SIZE];
    char phrase_to_process[MAX_PHRASE_SIZE];
    char key[KEY_SIZE + 1];
    char encrypted_result[MAX_PHRASE_SIZE];
    char decrypted_result[MAX_PHRASE_SIZE];

    // 1. Ler a frase original
    printf("Digite a frase: ");
    fgets(original_phrase, sizeof(original_phrase), stdin);
    original_phrase[strcspn(original_phrase, "\n")] = 0;

    // Adiciona o separador
    printf("\n========\n\n");

    // 2. Gerar a chave válida
    generate_key(original_phrase, key);
    printf("Chave gerada: \"%s\"\n", key);

    // 3. Preparar a frase para criptografar/descriptografar (limpar)
    to_uppercase_and_filter_invalid(original_phrase, phrase_to_process);

    // 4. Criptografar a frase preparada
    encrypt_phrase(phrase_to_process, key, encrypted_result);
    printf("Frase criptografada: \"%s\"\n\n", encrypted_result);

    // 5. Descriptografar para verificar
    decrypt_phrase(encrypted_result, key, decrypted_result);
    printf("Frase descriptografada: \"%s\"\n", decrypted_result);
    
    printf("\n========\n");

    return 0;
}


