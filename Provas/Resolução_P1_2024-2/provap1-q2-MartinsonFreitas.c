#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
 * string_to_bitmask: Recebe uma string de 32 caracteres e retorna um inteiro
 * onde cada bit é 1 se o caractere correspondente for maiúsculo, e 0 se for minúsculo.
 * O bit 0 (mais à direita) corresponde ao último caractere da string (índice 31).
 * O bit 31 (mais à esquerda) corresponde ao primeiro caractere da string (índice 0).
 * Esta implementação foi ajustada para corresponder ao exemplo da prova (Saída: DA0F8395).
 */
unsigned int string_to_bitmask(const char s[]) {
    unsigned int resultado = 0;
    int i;

    if (strlen(s) != 32) {
        fprintf(stderr, "ERRO: A string deve ter exatamente 32 caracteres.\n");
        return 0;
    }

    // i é o índice do bit (0 a 31)
    for (i = 0; i < 32; i++) {
        // O caractere correspondente é o de índice (31 - i)
        // i=0 -> s[31] -> bit 0
        // i=31 -> s[0] -> bit 31
        if (isupper((unsigned char)s[31 - i])) {
            resultado |= (1U << i);
        }
    }

    return resultado;
}

int main(void) {
    // Exemplo fornecido na prova: “SUpEEcAlifraGILISticexPIAliDoCiO”
    const char exemplo[] = "SUpEEcAlifraGILISticexPIAliDoCiO";
    unsigned int mascara;

    mascara = string_to_bitmask(exemplo);

    printf("String de entrada: %s\n", exemplo);
    printf("Saída esperada (Hexa): DA0F8395\n");
    printf("Máscara de bits (Hexa): %X\n", mascara);
    
    return 0;
}
