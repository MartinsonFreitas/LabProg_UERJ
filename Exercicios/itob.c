#include <limits.h>
#include <stdio.h>
#include <string.h>

/*
 * Escreva uma função itob(n, s, b) que converta o inteiro n em uma representação de caracteres com base b na string s. 
 * Em particular, itob(n, s, 16) formata n como um inteiro hexadecimal em s. 
 * 
 * Sua função consegue lidar corretamente com INT_MIN?
 * 
 * */

#define MAX 100

void itoa (int n, char s[]);
void itob(int n, char s[], int b);
void reverse(char s[]);


// Função auxiliar para rodar um teste
void test(int n, int b, const char *esperado) {
    char buf[MAX];
    itob(n, buf, b);
    if (strcmp(buf, esperado) == 0) {
        printf("n=%d, base=%d -> %s [OK]\n", n, b, buf);
    } else {
        printf("n=%d, base=%d -> %s [FALHOU, esperado=%s]\n", n, b, buf,
               esperado);
    }
}

int main(void) {
	char s[16];
	int n = 348;
	itob(n,s,16);
	printf("num = %s\n",s);
	
    // 20 testes diversos
    test(0, 2, "0");   // Teste 1
    test(0, 16, "0");  // Teste 2

    test(10, 2, "1010");  // Teste 3
    test(10, 8, "12");    // Teste 4
    test(10, 10, "10");   // Teste 5
    test(10, 16, "A");    // Teste 6

    test(-10, 2, "-1010");  // Teste 7
    test(-10, 10, "-10");   // Teste 8
    test(-10, 16, "-A");    // Teste 9

    test(255, 2, "11111111");  // Teste 10
    test(255, 8, "377");       // Teste 11
    test(255, 16, "FF");       // Teste 12

    test(-255, 2, "-11111111");  // Teste 13
    test(-255, 16, "-FF");       // Teste 14

    test(12345, 2, "11000000111001");  // Teste 15
    test(12345, 7, "50664");           // Teste 16
    test(12345, 36, "9IX");            // Teste 17

    test(-12345, 10, "-12345");  // Teste 18
    test(-12345, 16, "-3039");   // Teste 19

    test(INT_MAX, 16, "7FFFFFFF");  // Teste 20

    // Testes extras com INT_MIN
    test(INT_MIN, 10, "-2147483648");
    test(INT_MIN, 16, "-80000000");

    // ---- 10 testes com bases incomuns ----
    test(100, 3, "10201");  // 100 em base 3
    test(100, 4, "1210");   // 100 em base 4
    test(100, 5, "400");    // 100 em base 5
    test(100, 6, "244");    // 100 em base 6
    test(100, 9, "121");    // 100 em base 9

    test(-100, 11, "-91");      // -100 em base 11
    test(100, 12, "84");        // 100 em base 12
    test(1000, 20, "2A0");      // 1000 em base 20
    test(12345, 30, "DLF");     // 12345 em base 30
    test(-54321, 35, "-19C1");  // -54321 em base 35

    return 0;
}

// Função auxiliar para inverter ums string
void reverse(char s[]) {
    int i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itob(int n, char s[], int b) {
    int i = 0;
    int sign = n;
    unsigned int val;

    if (sign < 0) {
        val = -n; // Use unsigned para lidar com INT_MIN
    } else {
        val = n;
    }

    do {
        int digit = val % b;
        if (digit < 10) {
            s[i++] = digit + '0';
        } else {
            s[i++] = digit - 10 + 'A'; // Para dígitos A-Z
        }
        val /= b;
    } while (val > 0);

    if (sign < 0) {
        s[i++] = '-';
    }
    s[i] = '\0';
    reverse(s);
}

/* itoa: converte n para caracteres em s */
void itoa (int n, char s[]){
	int i, sinal;
	
	if ((sinal = n) < 0) { // registra sinal
	
		n = -n; // torna n positivo
		}
		
	i = 0;
	
	do { // gera dígitos em ordem invertida
		
		s[i++] = n % 10 + '0'; // obtém prox. dígito
		
	} while ((n /= 10) > 0); // deleta-o
	
	if (sinal < 0) {
		s[i++] = '-';
	}
	
	s[i] = '\0';
	reverse (s);
}

