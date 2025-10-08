/* * 
 * Copyright 2025 Martinson <martinson@martinson-VPCEB17FB>
 * */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// --- DEFINIÇÕES GLOBAIS ---
#define TAM_CHAVE 30
#define TAM_FRASE 256
//#define FILENAME_SIZE 100
#define CARACTERES_VALIDOS "ABCDEFGHIJKLMNOPQRSTUVWXYZ,. " // BIBLIOTECA DE CARACTERES VÁLIDOS

// ... protótipos das funções ...

// --- IMPLEMENTAÇÃO DAS FUNÇÕES ---

// ... (outras funções permanecem iguais) ...

/**
 * @brief c) Completa a chave com os caracteres que faltam.
 * VERSÃO MODIFICADA USANDO A MACRO.
 */
void complete_key(char* key) {
    // A declaração const char ALL_CHARS[] foi removida daqui.
    int len = strlen(key);

    // Usamos a macro ALL_VALID_CHARS diretamente.
    for (int i = 0; i < TAM_CHAVE; i++) {
        char char_to_check = CARACTERES_VALIDOS[i];
        // strchr retorna NULL se o caractere não for encontrado na string 'key'
        if (strchr(key, char_to_check) == NULL) {
            key[len++] = char_to_check;
        }
    }
    key[len] = '\0'; // Garante que a chave termine corretamente
}

// A função generate_key agora depende da nova versão de complete_key
void generate_key(const char* phrase, char* key) {
    // Esta função agora está mais simples, pois não precisa mais de uma
    // implementação complexa e pode ser dividida novamente.
    
    // 1. Filtra e remove duplicatas da frase de entrada
    char temp_key[TAM_FRASE] = {0};
    int k = 0;
    int seen[256] = {0};
    for (int i = 0; phrase[i] != '\0'; i++) {
        char current_char = toupper(phrase[i]);
        if (is_valid_char(current_char) && !seen[(unsigned char)current_char]) {
            temp_key[k++] = current_char;
            seen[(unsigned char)current_char] = 1;
        }
    }
    temp_key[k] = '\0';
    strcpy(key, temp_key);

    // 2. Completa a chave com os caracteres faltantes
    complete_key(key);
}


// ... (resto do código, incluindo a main, permanece o mesmo) ...
