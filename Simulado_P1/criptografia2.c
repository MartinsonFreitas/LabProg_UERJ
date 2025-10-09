/*
 * criptografia2.c
 * 
 * Copyright 2025 Martinson <martinson@martinson-VPCEB17FB>
 * */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define TAM_MAX_FRASE 256
#define TAM_CHAVE 29 // A-Z (26) + ',' (1) + '.' (1) + ' ' (1)
// ALTERAÇÃO: Adicionada uma macro para o conjunto de caracteres válidos.
// Isso centraliza a definição e melhora a legibilidade.
#define CARACTERES_VALIDOS "ABCDEFGHIJKLMNOPQRSTUVWXYZ,. "

// Protótipos das funções
void func_maiscula(char *str);
int obter_val_caract(char c);
void gerar_chave(const char *frase, char *chave); // ALTERAÇÃO: 'frase' agora é const
void encriptar_frase(const char *frase, const char *chave, char *frase_encriptada); // ALTERAÇÃO: 'frase' e 'chave' agora são const

// Chamada da função main
int main() {
    char ent_frase_chave[TAM_MAX_FRASE];
    char chave_gerada[TAM_CHAVE + 1];
    char frase_para_criptografar[TAM_MAX_FRASE];
    char saida_criptografada[TAM_MAX_FRASE];

    printf("\n\tDigite a frase para gerar a chave criptográfica: ");
    fgets(ent_frase_chave, TAM_MAX_FRASE, stdin);
    // ALTERAÇÃO: Corrigido o comentário para refletir a remoção do '\n'.
    ent_frase_chave[strcspn(ent_frase_chave, "\n")] = '\0'; // Remove '\n'

    gerar_chave(ent_frase_chave, chave_gerada);
    printf("\tChave gerada: \"%s\"\n", chave_gerada);

    printf("\n\tDigite a frase a ser criptografada: ");
    fgets(frase_para_criptografar, TAM_MAX_FRASE, stdin);
    frase_para_criptografar[strcspn(frase_para_criptografar, "\n")] = '\0'; // Remove '\n'

    encriptar_frase(frase_para_criptografar, chave_gerada, saida_criptografada);
    printf("\tFrase criptografada: \"%s\"\n\n", saida_criptografada);    

    return 0;
}

// Converte todas as letras de uma string para maiúsculas
void func_maiscula(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper(str[i]);
    }
}

// Função para obter o valor de um caractere
int obter_val_caract(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 1;
    } else if (c == ',') {
        return 27;
    } else if (c == '.') {
        return 28;
    } else if (c == ' ') {
        return 29;
    }
    return 0; // retorna se caractere inválido
}

// Gera a chave criptográfica a partir de uma frase
// ALTERAÇÃO: O parâmetro 'frase' agora é 'const char *' porque a função não deve modificá-lo.
// A cópia local foi removida para economizar memória e processamento.
void gerar_chave(const char *frase, char *chave) {
    int contador = 0;
    int ja_tem_caracter[256] = {0}; // Para marcar caracteres já vistos

    // Processa a frase para obter caracteres únicos e válidos
    for (int i = 0; frase[i] != '\0'; i++) {
        // A conversão para maiúscula é feita caractere por caractere, sem modificar a string original.
        char c = toupper(frase[i]);
        if ((c >= 'A' && c <= 'Z') || c == ',' || c == '.' || c == ' ') {
            if (!ja_tem_caracter[(int)c]) {
                chave[contador++] = c;
                ja_tem_caracter[(int)c] = 1;
            }
        }
    }
    // A string 'chave' agora contém a primeira parte.

    // Adiciona caracteres que faltam a chave
    for (int i = 0; CARACTERES_VALIDOS[i] != '\0'; i++) {
        char c = CARACTERES_VALIDOS[i];
        if (!ja_tem_caracter[(int)c]) {
            chave[contador++] = c;
            // Não é estritamente necessário marcar como visto aqui, mas é uma boa prática.
            ja_tem_caracter[(int)c] = 1; 
        }
    }
    chave[contador] = '\0'; // Finaliza a string da chave
}

// Criptografa uma frase usando a chave fornecida
// ALTERAÇÃO: 'frase' e 'chave' são 'const' para indicar que são apenas de leitura.
void encriptar_frase(const char *frase, const char *chave, char *frase_encriptada) {
    // ALTERAÇÃO: Removida a cópia desnecessária da frase.
    // A conversão para maiúscula é feita "on-the-fly" (em tempo real).
    int indice_chave = 0;
    int indice_encriptado = 0;

    for (int i = 0; frase[i] != '\0'; i++) {
        char caracter_original = frase[i];
        char caracter_maiusculo = toupper(caracter_original);
        int valor_caracter = obter_val_caract(caracter_maiusculo);

        if (valor_caracter == 0) { // Caractere invalido, copia o original
            frase_encriptada[indice_encriptado++] = caracter_original;
        } else {
            indice_chave = (indice_chave + valor_caracter) % TAM_CHAVE;
            frase_encriptada[indice_encriptado++] = chave[indice_chave];
        }
    }
    frase_encriptada[indice_encriptado] = '\0';
}
