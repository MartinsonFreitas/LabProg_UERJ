#include <stdio.h>
#include <stdlib.h>

//definição de constantes
#define TRUE 1
#define FALSE 0

int isPrime(int n);

// Função main
int main() {
    
    // Questão 1: Verifica se eh primo...
    int num_primo;
    char numero[20]; // Buffer para armazenar a entrada como string
    
    // --- Leitura e conversão do valor de entrada (fgets + atoi) ---
    printf("Digite um numero: ");
    if (fgets(numero, sizeof(numero), stdin) != NULL) 
    {
        // Converte a string lida para inteiro
        num_primo = atoi(numero);
    } 
    else 
    {
        printf("Erro na leitura do numero.\n");
    }
    
    if (isPrime(num_primo) == TRUE) {
        printf("%d é um número primo.\n", num_primo);
    } else {
        printf("%d não é um número primo.\n", num_primo);
    }
     
    return 0;
}


// verifica se eh primo
int isPrime(int n) {
    if (n <= 1) {
        return FALSE; // Números menores ou iguais a 1 não são primos.
    }
    if (n <= 3) {
        return TRUE; // 2 e 3 são primos.
    }
    // Otimização: Ignora múltiplos de 2 e 3, verificando a partir de 5.
    if (n % 2 == 0 || n % 3 == 0) {
        return FALSE;
    }
    
    // Verifica divisores da forma 6k ± 1 até a raiz quadrada de n.
    for (int i = 5; (long long)i * i <= n; i = i + 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return FALSE;
        }
    }
    return TRUE;
}
