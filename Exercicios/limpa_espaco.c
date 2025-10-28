#include <stdio.h>

//Escreva um programa em C que copie sua entrada na saída, 
//trocando cada cadeia de dois ou mais espaços por um único espaço.

#include <stdio.h>

int main() {
    int caracter;
    int espaco = 0;

    while ((caracter = getchar()) != EOF) {
        if (caracter == ' ') {
            if (!espaco) {
                putchar(caracter);// imprime o primeiro espaço
                espaco = 1;// marca que o último foi espaço
            }
        // se já foi espaço, ignora
        } else {
            putchar(caracter);// imprime o caractere diferente de espaço
            espaco = 0;    // reseta a flag
        }
    }

    return 0;
}

