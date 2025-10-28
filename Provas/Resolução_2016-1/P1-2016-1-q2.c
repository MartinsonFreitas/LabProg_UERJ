/*
 * Solução para a Questão 2 da Prova P1 - 2016-1 (FEN06-04049 - Laboratório de Programação I)
 *
 * Objetivo: Implementar uma criptografia por transposição contínua com incremento
 *           variável e contagem de ocorrências de caracteres.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXLINHA 1000
#define ALFABETO_TAM 26

// Protótipos das funções
int lelinha (char s[], int lim);
char criptografa_char(char c);
void criptografa_linha(char linha[]);

// Contagem de vezes que cada letra (A-Z ou a-z) apareceu.
// O índice 0 é para 'A'/'a', 1 para 'B'/'b', ..., 25 para 'Z'/'z'.
static int contagem_ocorrencias[ALFABETO_TAM] = {0};

/* Função main. */
int main(void)
{
    char linha[MAXLINHA];

    printf("Início da Criptografia (digite uma linha vazia para terminar):\n");

    // Loop para ler e criptografar múltiplas linhas
    while (lelinha(linha, MAXLINHA) != 0) {
        criptografa_linha(linha);
    }

    printf("\nCriptografia finalizada.\n");

    return 0;
}


/* lelinha: lê uma linha do teclado, armazena em 's' e retorna o tamanho. */
int lelinha (char s[], int lim)
{
    int c, i;

    // Loop para ler caracteres até o limite, EOF, ou quebra de linha
    for(i = 0; i < lim - 1 && (c = getchar()) != '\n' && c != EOF; ++i) {
        s[i] = c;
    }

    // Se o último caractere lido foi '\n', armazena-o
    if (c == '\n') {
        s[i] = c;
        ++i;
    }

    // Termina a string com o caractere nulo
    s[i] = '\0';

    // Uma linha vazia é aquela que tem tamanho 0 (EOF) ou tamanho 1 (apenas '\n').
    if (i == 0 || (i == 1 && s[0] == '\n')) {
        return 0; // Linha vazia
    }
    return 1; // Linha não vazia
}

/* criptografa_char: Criptografa um único caractere. */
char criptografa_char(char c)
{
    int indice;
    int deslocamento;
    char base;
    char novo_char = c;

    if (isalpha(c)) {
        if (isupper(c)) {
            base = 'A';
        } else {
            base = 'a';
        }

        // Determina o índice da letra no alfabeto (0 a 25)
        indice = c - base;

        // Aplica a transposição
        deslocamento = contagem_ocorrencias[indice];

        // Aplica o deslocamento
        novo_char = base + (indice + deslocamento) % ALFABETO_TAM;

       //    A contagem é reiniciada ao atingir 26.
        contagem_ocorrencias[indice] = (contagem_ocorrencias[indice] + 1) % ALFABETO_TAM;

    }

    return novo_char;
}

/* criptografa_linha: Criptografa a linha de entrada e imprime o resultado. */
void criptografa_linha(char linha[])
{
    int i;
    for (i = 0; linha[i] != '\0'; ++i) {
        // Não criptografa a quebra de linha, se houver
        if (linha[i] != '\n') {
            putchar(criptografa_char(linha[i]));
        } else {
            putchar(linha[i]); // Imprime a quebra de linha
        }
    }
}
