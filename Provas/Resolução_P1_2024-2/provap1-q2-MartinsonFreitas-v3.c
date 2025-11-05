#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
 * string_to_bitmask: Versão simplificada. Retorna 'int' e aceita 'char[]' não constante.
 * RISCO: O valor de retorno será negativo se o bit 31 (mais à esquerda) for 1.
 */
int string_to_bitmask(char s[]) {
    int resultado = 0;
    int indice_string; 

    // O 32 aqui é assumido como o tamanho correto
    if (strlen(s) != 32) {
        fprintf(stderr, "ERRO: A string deve ter exatamente 32 caracteres.\n");
        return 0;
    }

    // Percorre a string da primeira letra (índice 0) até a última (índice 31)
    for (indice_string = 0; indice_string < 32; indice_string++) {
        // bit_pos: 0 -> 31, 31 -> 0
        int bit_pos = 31 - indice_string;
        
        // Verifica se o caractere é maiúsculo
        if (isupper((unsigned char)s[indice_string])) {
            // Seta o bit correspondente (1 << 31 é comportamento indefinido, mas 
            // geralmente funciona em sistemas de 32 bits, resultando em um int negativo)
            resultado |= (1 << bit_pos);
        }
    }

    return resultado;
}

int main(void) {
    // Declaração de array de char não constante
    char exemplo[] = "SUpEEcAlifraGILISticexPIAliDoCiO"; 
    int mascara; // Variável 'int' para o resultado

    mascara = string_to_bitmask(exemplo);

    printf("String de entrada: %s\n", exemplo);
    printf("Saída esperada (Hexa): DA0F8395\n");
    // Usamos %X para garantir que o 'int' seja impresso como um valor hexadecimal
    // de 32 bits, ignorando o sinal (o que é o mais próximo do correto).
    printf("Máscara de bits (Hexa): %X\n", mascara);
    
    return 0;
}
