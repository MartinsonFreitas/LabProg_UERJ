/* * 
 * Copyright 2025 Martinson <martinson@martinson-VPCEB17FB>
 * */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define KEY_SIZE 29
#define MAX_PHRASE_SIZE 256
#define FILENAME_SIZE 100

// --- Protótipos de todas as funções ---
void limpar_buffer_entrada();
void exibir_menu();
int get_char_value(char c);
void generate_key(const char* phrase, char* key);
void to_uppercase_and_filter_invalid(const char* input, char* output);
void encrypt_phrase(const char* phrase, const char* key, char* encrypted_phrase);
void decrypt_phrase(const char* encrypted_phrase, const char* key, char* decrypted_phrase);
int salvar_chave_em_arquivo(const char* filename, const char* key);
int ler_chave_de_arquivo(const char* filename, char* key);

// --- Implementação das funções (lógica principal inalterada) ---
// ... (as funções de criptografia, etc., são as mesmas de antes) ...
int get_char_value(char c) { if (c >= 'A' && c <= 'Z') return c - 'A' + 1; if (c == ',') return 27; if (c == '.') return 28; if (c == ' ') return 29; return 0; }
void generate_key(const char* phrase, char* key) {
    char temp_key[MAX_PHRASE_SIZE] = {0}; int k = 0; int seen[256] = {0};
    for (int i = 0; phrase[i] != '\0'; i++) { char current_char = toupper(phrase[i]); if ((isalpha(current_char) || current_char == ',' || current_char == '.' || current_char == ' ') && !seen[(unsigned char)current_char]) { temp_key[k++] = current_char; seen[(unsigned char)current_char] = 1; } }
    temp_key[k] = '\0'; const char ALL_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ,. "; int len = strlen(temp_key);
    for (int i = 0; i < KEY_SIZE; i++) { if (strchr(temp_key, ALL_CHARS[i]) == NULL) { temp_key[len++] = ALL_CHARS[i]; } }
    temp_key[len] = '\0'; strncpy(key, temp_key, KEY_SIZE); key[KEY_SIZE] = '\0';
}
void to_uppercase_and_filter_invalid(const char* input, char* output) { int k = 0; for (int i = 0; input[i] != '\0'; i++) { char current_char = toupper(input[i]); if (get_char_value(current_char) > 0) { output[k++] = current_char; } } output[k] = '\0'; }
void encrypt_phrase(const char* phrase, const char* key, char* encrypted_phrase) { int key_index = 0; int output_index = 0; for (int i = 0; phrase[i] != '\0'; i++) { int value = get_char_value(phrase[i]); if (value > 0) { key_index = (key_index + value) % KEY_SIZE; encrypted_phrase[output_index++] = key[key_index]; } else { encrypted_phrase[output_index++] = phrase[i]; } } encrypted_phrase[output_index] = '\0'; }
void decrypt_phrase(const char* encrypted_phrase, const char* key, char* decrypted_phrase) {
    int prev_key_index = 0; int output_index = 0;
    for (int i = 0; encrypted_phrase[i] != '\0'; i++) {
        int current_key_index = -1; for(int j=0; j<KEY_SIZE; ++j) { if(key[j] == encrypted_phrase[i]) { current_key_index = j; break; } }
        if (current_key_index != -1) { int original_value = (current_key_index - prev_key_index + KEY_SIZE) % KEY_SIZE; if (original_value == 0) original_value = KEY_SIZE;
            char original_char = '\0'; if (original_value >= 1 && original_value <= 26) original_char = 'A' + original_value - 1; else if (original_value == 27) original_char = ','; else if (original_value == 28) original_char = '.'; else if (original_value == 29) original_char = ' ';
            if (original_char != '\0') { decrypted_phrase[output_index++] = original_char; } prev_key_index = current_key_index;
        } else { decrypted_phrase[output_index++] = encrypted_phrase[i]; }
    } decrypted_phrase[output_index] = '\0';
}


/* -------------------------------------------------------------------------- */
/*                  FUNÇÕES DE MANIPULAÇÃO DE ARQUIVO                         */
/* -------------------------------------------------------------------------- */

/**
 * @brief Salva a chave em um arquivo de texto.
 * @return 1 em sucesso, 0 em falha.
 */
int salvar_chave_em_arquivo(const char* filename, const char* key) {
    FILE* fp = fopen(filename, "w"); // "w" para escrever (cria/sobrescreve o arquivo)
    if (fp == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        return 0;
    }
    fprintf(fp, "%s", key);
    fclose(fp);
    return 1;
}

/**
 * @brief Lê uma chave de um arquivo de texto.
 * @return 1 em sucesso, 0 em falha.
 */
int ler_chave_de_arquivo(const char* filename, char* key) {
    FILE* fp = fopen(filename, "r"); // "r" para ler
    if (fp == NULL) {
        perror("Erro ao abrir o arquivo para leitura");
        return 0;
    }
    if (fgets(key, KEY_SIZE + 1, fp) == NULL) {
        fprintf(stderr, "Erro ao ler a chave do arquivo ou arquivo vazio.\n");
        fclose(fp);
        return 0;
    }
    fclose(fp);
    // Garante que a chave tenha o tamanho correto e seja nula-terminada
    key[KEY_SIZE] = '\0';
    if (strlen(key) != KEY_SIZE) {
        fprintf(stderr, "Aviso: A chave no arquivo nao tem o tamanho esperado de %d caracteres.\n", KEY_SIZE);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/*                         FUNÇÕES DO MENU INTERATIVO                         */
/* -------------------------------------------------------------------------- */

void limpar_buffer_entrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void exibir_menu() {
    printf("\n\n=============== CRIPTO-TOOL ===============\n");
    printf("1. Gerar e Salvar Chave em Arquivo\n");
    printf("2. Criptografar com Chave de Arquivo\n");
    printf("3. Descriptografar com Chave de Arquivo\n");
    printf("4. Sair\n");
    printf("===========================================\n");
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
    char nome_arquivo[FILENAME_SIZE];

    while (1) {
        exibir_menu();
        if (scanf("%d", &escolha) != 1) {
            printf("\nEntrada invalida. Por favor, digite um numero.\n");
            limpar_buffer_entrada();
            continue;
        }
        limpar_buffer_entrada();

        switch (escolha) {
            case 1: // Gerar e Salvar Chave
                printf("\n--- Gerar e Salvar Chave ---\n");
                printf("Digite a frase para gerar a chave: ");
                fgets(frase_base, sizeof(frase_base), stdin);
                frase_base[strcspn(frase_base, "\n")] = 0;

                generate_key(frase_base, chave);
                printf("Chave gerada: \"%s\"\n", chave);

                printf("Digite o nome do arquivo para salvar a chave (ex: chave.txt): ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                nome_arquivo[strcspn(nome_arquivo, "\n")] = 0;

                if (salvar_chave_em_arquivo(nome_arquivo, chave)) {
                    printf("Chave salva com sucesso em \"%s\"\n", nome_arquivo);
                }
                break;

            case 2: // Criptografar com Chave de Arquivo
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

            case 3: // Descriptografar com Chave de Arquivo
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

            case 4: // Sair
                printf("\nSaindo do programa. Ate mais!\n");
                return 0;

            default:
                printf("\nOpcao invalida! Por favor, escolha uma das opcoes do menu.\n");
                break;
        }
    }

    return 0;
}


