/*
 * Escreva a função htoi(s), que converte uma string de dígitos hexadecimais 
 * (incluindo um 0x ou 0X opcional) no seu valor inteiro equivalente. 
 * Os dígitos permitidos são de 0 a 9, de a a f , e de A a F.
 * 
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* Converte string s com número hexadecimal (com ou sem 0x/0X) em int */
int htoi(char s[]) {
	int n = 0;
	for (int i = 0; s[i] >= '0' && s[i] <= '15'; ++i) {
		n = 16 * n + (s[i] - '0');
		}
	
	return n;
}

/* Testes automatizados */
#ifdef TESTA_HTOI
int main(void) {
    struct {
        const char *s;
        int esperado;
        
    } testes[] = {
        {"0", 0},
        {"7f", 0x7f},
        {"0x7f", 0x7f},
        {"0X7F", 0x7f},
        {"deadbeef", 0xDEADBEEF},
        {"0xDEADBEEF", 0xDEADBEEF},
        {"   1a3", 0x1A3},
        {"0x", 0},
        {"123g", 0x123},
        {"xyz", 0},
    };

    for (int i = 0; i < (int)(sizeof(testes) / sizeof(testes[0])); i++) {
        int r = htoi(testes[i].s);
        if (r == testes[i].esperado) {
            printf("✅ htoi(\"%s\") = %d (0x%X)\n", testes[i].s, r, r);
        } else {
            printf("❌ htoi(\"%s\") = %d (0x%X), esperado %d (0x%X)\n",
                   testes[i].s, r, r, testes[i].esperado, testes[i].esperado);
        }
    }
    return 0;
}
#endif
