#include <ctype.h>
#include <stdio.h>

/* 
 * Escreva a função htoi(s), que converte uma string de dígitos hexadecimais (incluindo um 0x ou 0X opcional) 
 * no seu valor inteiro equivalente. Os dígitos permitidos são de 0 a 9, de a a f , e de A a F. 
 */
 
int htoi(const char s[]) {
	int i, n, hex;
    
    i = 0;
    n = 0;
    
    // Pula espaços em branco
    // isspace => verifica espaços em braco
    while (isspace(s[i])) {
        i++;
    }

    // Pula "0x" ou "0X"
    if (s[i] == '0' && (s[i+1] == 'x' || s[i+1] == 'X')) {
        i = 2;
    }

    // Processa hexadecimais
    // operação de conversão
    // isxdigit = verifica se é um dígito hexadecimal válido
	while (s[i] != '\0' && isxdigit(s[i])) {
		if (isdigit(s[i])) {
            hex = s[i] - '0'; 
        } else if (islower(s[i])) {
            hex = s[i] - 'a' + 10;
        } else if (isupper(s[i])) {
            hex = s[i] - 'A' + 10;
        } 
        /* atoi: converte s em um inteiro */
		//n = 10 * n + (s[i] - '0'); 
        n = n * 16 + hex;
        i++;
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
