#include <stdio.h>

//Escreva um programa que imprima sua entrada com uma palavra por linha.

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
			printf("\n");
			espaco = 0;    // reseta a flag		
			
        // se já foi espaço, ignora
        } else {
            putchar(caracter);// imprime o caractere diferente de espaço
            espaco = 0;    // reseta a flag            
        }
        
    }
	printf("\n");
    return 0;
}
