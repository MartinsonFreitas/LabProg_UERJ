/*
 * Escreva uma função setabits(x, p, n, y) que retorne x com os n bits que começam na posição p 
 * setados com os n bits mais à direita de y, deixando os outros bits inalterados.
 * 
 */

#include <stdio.h>

/* A função obtembits do livro */
unsigned obtembits(unsigned x, int p, int n) {
    return (x >> (p + 1 - n)) & ~(~0U << n);
}

unsigned setabits(unsigned x, int p, int n, unsigned y) {
	// escreva seu código aqui
	
    // 1. Obtem os bits de 'x' acima da posição 'p' e os move para a posição correta
    unsigned upper_bits_x = (p == 31) ? 0 : obtembits(x, 31, 31 - p);
    upper_bits_x <<= (p + 1);
    
    // 2. Obtem os bits de 'x' abaixo da posição (p - n + 1) e os move para a posição correta
    unsigned lower_bits_x = (p - n + 1 == 0) ? 0 : obtembits(x, p - n, p - n + 1);

    // 3. Obtem os n bits mais à direita de 'y'
    unsigned y_bits = obtembits(y, n - 1, n);
    y_bits <<= (p - n + 1);

    // 4. Combina os três segmentos: os bits superiores de 'x', os bits de 'y' e os bits inferiores de 'x'
    return upper_bits_x | y_bits | lower_bits_x;
}

/* função auxiliar para imprimir binário */
void printbin(unsigned x) {
    for (int i = 31; i >= 0; i--) {
        putchar((x & (1U << i)) ? '1' : '0');
        if (i % 4 == 0) putchar(' ');
    }
}

int main(void) {
    struct {
        unsigned x;
        int p, n;
        unsigned y;
        unsigned esperado;
    } testes[] = {
        // Exemplo: substituir bits 4..2 de x=11010110 (214) pelos 3 bits de y=101 (5)
        {0b11010110, 4, 3, 0b101, 0b11010110},

        // Substituir bits 7..4 de x=11010110 pelos 4 bits de y=0000
        {0b11010110, 7, 4, 0b0000, 0b00000110},

        // Substituir bits 2..0 de x=101011 pelos 3 bits de y=111
        {0b101011, 2, 3, 0b111, 0b101111},

        // Substituir bits 15..8 de 0xDEADBEEF pelos 8 bits de y=0xAA
        {0xDEADBEEF, 15, 8, 0xAA, (0xDEADAAEF)},

        // Substituir bits 5..4 de x=11110000 pelos 2 bits de y=01
        {0b11110000, 5, 2, 0b01, 0b11010000},
    };

    int total = sizeof(testes) / sizeof(testes[0]);

    for (int i = 0; i < total; i++) {
        unsigned r =
            setabits(testes[i].x, testes[i].p, testes[i].n, testes[i].y);

        if (r == testes[i].esperado) {
            printf("✅ Teste %d passou: ", i + 1);
        } else {
            printf("❌ Teste %d FALHOU: ", i + 1);
        }

        printf("x=");
        printbin(testes[i].x);
        printf(" p=%d n=%d y=%u → obtido=%u (0x%X), esperado=%u (0x%X)\n",
               testes[i].p, testes[i].n, testes[i].y, r, r, testes[i].esperado,
               testes[i].esperado);
    }

    return 0;
}
