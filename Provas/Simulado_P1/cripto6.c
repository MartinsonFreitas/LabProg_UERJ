/* * 
 * Copyright 2025 Martinson <martinson@martinson-VPCEB17FB>
 * */


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// --- DEFINIÇÕES GLOBAIS ---
#define KEY_SIZE 29
#define MAX_PHRASE_SIZE 256
#define FILENAME_SIZE 100
#define ALL_VALID_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZ,. "

// --- Protótipos das funções ---
void limpar_buffer_entrada();
void exibir_menu();
int get_char_value(char c);
int is_valid_char(char c);
void filter_and_remove_duplicates(const char* input, char* output);
void complete_key(char* key);
void generate_key(const char* phrase, char* key);
void to_uppercase_and_filter_invalid(const char* input, char* output);
void encrypt_phrase(const char* phrase, const char* key, char* encrypted_phrase);
void decrypt_phrase(const char* encrypted_phrase, const char* key, char* decrypted_phrase);
int salvar_chave_em_arquivo(const char* filename, const char* key);
int ler_chave_de_arquivo(const char* filename, char* key);

// --- Implementação das funções ---

// ... (funções como get_char_value, is_valid_char, etc. permanecem as mesmas) ...
int get_char_value(char c) { if (c >= 'A' && c <= 'Z') return c - 'A' + 1; if (c == ',') return 27; if (c == '.') return 28; if (c == ' ') return 29; return 0; }
int is_valid_char(char c) { return (isalpha(c) || c == ',' || c == '.' || c == ' '); }
void to_uppercase_and_filter_invalid(const char* input, char* output) { int k = 0; for (int i = 0; input[i] != '\0'; i++) { char current_char = toupper(input[i]); if (is_valid_char(current_char)) { output[k++] = current_char; } } output[k] = '\0'; }
void encrypt_phrase(const char* phrase, const char* key, char* encrypted_phrase) { int key_index = 0; int output_index = 0; for (int i = 0; phrase[i] != '\0'; i++) { int value = get_char_value(phrase[i]); if (value > 0) { key_index = (key_index + value) % KEY_SIZE; encrypted_phrase[output_index++] = key[key_index]; } else { encrypted_phrase[output_index++] = phrase[i]; } } encrypted_phrase[output_index] = '\0'; }
void decrypt_phrase(const char* encrypted_phrase, const char* key, char* decrypted_phrase) { int prev_key_index = 0; int output_index = 0; for (int i = 0; encrypted_phrase[i] != '\0'; i++) { int current_key_index = -1; for(int j=0; j<KEY_SIZE; ++j) { if(key[j] == encrypted_phrase[i]) { current_key_index = j; break; } } if (current_key_index != -1) { int original_value = (current_key_index - prev_key_index + KEY_SIZE) % KEY_SIZE; if (original_value == 0) original_value = KEY_SIZE; char original_char = '\0'; if (original_value >= 1 && original_value <= 26) original_char = 'A' + original_value - 1; else if (original_value == 27) original_char = ','; else if (original_value == 28) original_char = '.'; else if (original_value == 29) original_char = ' '; if (original_char != '\0') { decrypted_phrase[output_index++] = original_char; } prev_key_index = current_key_index; } else { decrypted_phrase[output_index++] = encrypted_phrase[i]; } } decrypted_phrase[output_index] = '\0'; }

/* -------------------------------------------------------------------------- */
/*                  FUNÇÕES DE GERAÇÃO DE CHAVE (REFATORADAS)                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief Converte para maiúsculas, remove caracteres inválidos e duplicados.
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
 * @brief Completa a chave com os caracteres que faltam do conjunto padrão.
 */
void complete_key(char* key) {
    int len = strlen(key);

    for (int i = 0; i < KEY_SIZE; i++) {
        char char_to_check = ALL_VALID_CHARS[i];
        if (strchr(key, char_to_check) == NULL) {
            key[len++] = char_to_check;
        }
    }
    key[len] = '\0';
}

/**
 * @brief Função orquestradora que gera a chave válida.
 * É mais limpa e apenas chama as funções de apoio.
 */
void generate_key(const char* phrase, char* key) {
    // Passo 1: Cria a base da chave a partir da frase (sem duplicatas)
    filter_and_remove_duplicates(phrase, key);

    // Passo 2: Completa a chave com os caracteres que faltam
    complete_key(key);
}


/* -------------------------------------------------------------------------- */
/*                  FUNÇÕES DE ARQUIVO E MENU (SEM ALTERAÇÕES)                */
/* -------------------------------------------------------------------------- */
int salvar_chave_em_arquivo(const char* filename, const char* key) { FILE* fp = fopen(filename, "w"); if (fp == NULL) { perror("Erro ao abrir o arquivo para escrita"); return 0; } fprintf(fp, "%s", key); fclose(fp); return 1; }
int ler_chave_de_arquivo(const char* filename, char* key) { FILE* fp = fopen(filename, "r"); if (fp == NULL) { perror("Erro ao abrir o arquivo para leitura"); return 0; } if (fgets(key, KEY_SIZE + 1, fp) == NULL) { fprintf(stderr, "Erro ao ler a chave do arquivo.\n"); fclose(fp); return 0; } fclose(fp); key[KEY_SIZE] = '\0'; return 1; }
void limpar_buffer_entrada() { int c; while ((c = getchar()) != '\n' && c != EOF); }
void exibir_menu() { printf("\n\n=============== CRIPTO-TOOL ===============\n"); printf("1. Gerar e Salvar Chave em Arquivo\n"); printf("2. Criptografar com Chave de Arquivo\n"); printf("3. Descriptografar com Chave de Arquivo\n"); printf("4. Sair\n"); printf("===========================================\n"); printf("Escolha uma opcao: "); }

/* -------------------------------------------------------------------------- */
/*                 FUNÇÃO PRINCIPAL (SEM ALTERAÇÕES)                          */
/* -------------------------------------------------------------------------- */
int main() {
    int escolha;
    char frase_base[MAX_PHRASE_SIZE];
    char frase_processada[MAX_PHRASE_SIZE];
    char chave[KEY_SIZE + 1];
    char resultado[MAX_PHRASE_SIZE];
    char nome_arquivo[FILENAME_SIZE];

    while (1) {
        exibir_menu();
        if (scanf("%d", &escolha) != 1) { printf("\nEntrada invalida. Por favor, digite um numero.\n"); limpar_buffer_entrada(); continue; }
        limpar_buffer_entrada();

        switch (escolha) {
            case 1:
                printf("\n--- Gerar e Salvar Chave ---\n");
                printf("Digite a frase para gerar a chave: ");
                fgets(frase_base, sizeof(frase_base), stdin);
                frase_base[strcspn(frase_base, "\n")] = 0;
                generate_key(frase_base, chave);
                printf("Chave gerada: \"%s\"\n", chave);
                printf("Digite o nome do arquivo para salvar a chave (ex: chave.txt): ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                nome_arquivo[strcspn(nome_arquivo, "\n")] = 0;
                if (salvar_chave_em_arquivo(nome_arquivo, chave)) { printf("Chave salva com sucesso em \"%s\"\n", nome_arquivo); }
                break;
            case 2:
                printf("\n--- Criptografar com Chave de Arquivo ---\n");
                printf("Digite o nome do arquivo da chave: ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                nome_arquivo[strcspn(nome_arquivo, "\n")] = 0;
                if (ler_chave_de_arquivo(nome_arquivo, chave)) {
                    printf("Chave carregada com sucesso.\n");
                    printf("Digite a frase a ser criptografada: ");
                    fgets(frase_base, sizeof(frase_base), stdin);
                    frase_base[strcspn(frase_base, "\n")] = 0;
                    to_uppercase_and_filter_invalid(frase_base, frase_processada);
                    encrypt_phrase(frase_processada, chave, resultado);
                    printf("Frase criptografada: \"%s\"\n", resultado);
                }
                break;
            case 3:
                printf("\n--- Descriptografar com Chave de Arquivo ---\n");
                printf("Digite o nome do arquivo da chave: ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                nome_arquivo[strcspn(nome_arquivo, "\n")] = 0;
                if (ler_chave_de_arquivo(nome_arquivo, chave)) {
                    printf("Chave carregada com sucesso.\n");
                    printf("Digite a frase criptografada: ");
                    fgets(frase_base, sizeof(frase_base), stdin);
                    frase_base[strcspn(frase_base, "\n")] = 0;
                    decrypt_phrase(frase_base, chave, resultado);
                    printf("Frase descriptografada: \"%s\"\n", resultado);
                }
                break;
            case 4:
                printf("\nSaindo do programa. Ate mais!\n");
                return 0;
            default:
                printf("\nOpcao invalida! Por favor, escolha uma das opcoes do menu.\n");
                break;
        }
    }
    return 0;
}


