/*
 * Solução para a Questão 1 da Prova P1 - 2016-1 (FEN06-04049 - Laboratório de Programação I)
 *
 * Objetivo: Ler uma linha do teclado e imprimi-la na tela de trás para frente.
 */

#include <stdio.h>
#include <string.h>

#define MAXLINHA 1000 // Tamanho máximo da linha de entrada

// protótipos das funções
int lelinha (char s[], int lim);
void inverte (char s[]);

/* Função main */
int main(void)
{
    char linha[MAXLINHA];
    int tam;

    // O problema pede para ler "uma linha", então lemos apenas uma vez.
    printf("Digite a frase: ");
    tam = lelinha(linha, MAXLINHA);

    if (tam > 0) {
        printf("A frase invertida é: ");
        inverte(linha);
    }

    return 0;
}

/* lelinha: lê uma linha do teclado, armazena em 's' e retorna o tamanho.
 * Baseado na função lelinha dos exemplos do repositório. */
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
    return i;
}

/* inverte: inverte a string 's' e a imprime. */
void inverte (char s[])
{
    int i;
    // Pega o tamanho da string, ignorando o '\0' e o '\n' (se presente)
    int len = strlen(s);

    // Se a string terminar em '\n', não o incluímos na inversão
    if (len > 0 && s[len - 1] == '\n') {
        len--;
    }

    // Imprime a string de trás para frente
    for (i = len - 1; i >= 0; --i) {
        putchar(s[i]);
    }
    // Adiciona uma nova linha no final da saída
    putchar('\n');
}
