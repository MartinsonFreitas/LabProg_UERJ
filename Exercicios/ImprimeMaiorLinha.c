#include <stdio.h>

#define MAXLINHA 10 //tamanho máximo da linha entrada

int lelinha (char [], int);
void copia (char [], char []);

/* imprime maior linha entrada */
int main(void)
{
    int tam;               // tamanho atual da linha
    int max = 0;           // tamanho máximo visto até agora
    char linha [MAXLINHA]; // linha atual
    char maior [MAXLINHA]; // maior linha guardada

    while ((tam = lelinha (linha, MAXLINHA)) > 0) {
        if (tam > max) {
            max = tam;
            copia (maior, linha);
        }
    }

    if (max > 0) { // entrada tinha uma linha
        printf ("%s", maior);
    }

    return 0;
}

/* lelinha: lê uma linha em s, retorna tamanho */
int lelinha (char s[], int lim)
{
    int c, i;

    for(i = 0; i < lim - 1 && (c = getchar()) != '\n' && c != EOF; ++i) {
        s[i] = c;
    }

    if (c == '\n') {
        s[i] = c;
        ++i;
    }

    s[i] = '\0';
    return i;
}

/* copia: copia 'de' -> 'para';
   presume que 'para' é grande o suficiente */
void copia (char para[], char de[])
{
    int i = 0;

    while ((para[i] = de[i]) != '\0') {
        ++i;
    }
}
