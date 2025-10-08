/*
 * decripto.c
 * 
 * Copyright 2025 Martinson <martinson@martinson-VPCEB17FB>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
 
 #include <stdio.h>
#include <string.h>
#include <ctype.h>

#define KEY_SIZE 29
#define MAX_PHRASE_SIZE 256

// ... (todas as funções anteriores: get_char_value, generate_key, etc.) ...

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
        } else {
            // Mantém caracteres que não fazem parte da criptografia (ex: quebra de linha)
            decrypted_phrase[output_index++] = current_char;
        }
    }
    decrypted_phrase[output_index] = '\0';
}


int main() {
    // ... (código para gerar a chave e criptografar) ...
    
    char decrypted_result[MAX_PHRASE_SIZE];

    // 6. Descriptografar a mensagem para verificar
    decrypt_phrase(encrypted_result, key, decrypted_result);
    printf("\nFrase descriptografada: \"%s\"\n", decrypted_result);

    return 0;
}

