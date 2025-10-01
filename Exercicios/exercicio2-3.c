#include <stdio.h>

//Escreva um programa que imprima sua entrada com uma palavra por linha.
//1seguindo o exemplo do livro
#define dentro 1
#define fora 0

int main() {
    int c;
    int estado = fora;

    while ((c = getchar()) != EOF) {
        // Verifica se é espaço, tabulação ou quebra de linha
        if (c == ' ' || c == '\t' || c == '\n') {
            if (estado == dentro) {
                // Final de uma palavra
                putchar('\n');
                estado = fora;
            }
            // Ignora múltiplos espaços
        } else {
            // Parte de uma palavra
            putchar(c);
            estado = dentro;
        }
    }

    return 0;
}
