#include <stdio.h>

//Escreva um programa que imprima sua entrada com uma palavra por linha.

int main() {
    int c;
    int dentro = 0;

    while ((c = getchar()) != EOF) {
        // Verifica se é espaço, tabulação ou quebra de linha
        if (c == ' ' || c == '\t' || c == '\n') {
            if (dentro) {
                // Final de uma palavra
                putchar('\n');
                dentro = 0;
            }
            // Ignora múltiplos espaços
        } else {
            // Parte de uma palavra
            putchar(c);
            dentro = 1;
        }
    }

    return 0;
}
