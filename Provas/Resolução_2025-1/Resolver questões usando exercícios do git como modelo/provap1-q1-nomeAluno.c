#include <stdio.h>
#include <math.h>

/*
 * eh_primo: verifica se um número é primo.
 * Retorna 1 se n for primo, 0 caso contrário.
 */
/*
 * eh_primo: verifica se um número é primo.
 * Retorna 1 se n for primo, 0 caso contrário.
 */
int eh_primo(int n)
{
    if (n <= 1) return 0;
    if (n <= 3) return 1;

    if (n % 2 == 0 || n % 3 == 0) return 0;

    // Verifica divisores a partir de 5, de 6 em 6 (5, 7, 11, 13, ...)
    // Usando i*i <= n para evitar a função sqrt()
    int i;
    for (i = 5; i * i <= n; i = i + 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return 0;
        }
    }

    return 1;
}


/*
 * soma_primos: calcula a soma de todos os números primos entre a e b (inclusive).
 * Assume que a e b são inteiros positivos.
 */
long long soma_primos(int a, int b)
{
    long long soma = 0;
    int inicio, fim;

    // Garante que 'inicio' seja o menor e 'fim' o maior.
    if (a < b) {
        inicio = a;
        fim = b;
    } else {
        inicio = b;
        fim = a;
    }

    // Itera do menor ao maior, verificando a primalidade.
    int i;
    for (i = inicio; i <= fim; ++i) {
        if (eh_primo(i)) {
            soma += i;
        }
    }

    return soma;
}

/*
 * main: coordena a leitura dos números e a impressão do resultado.
 */
int main(void)
{
    int num1, num2;
    long long resultado;

    // Leitura dos 2 números.
    printf("Digite o primeiro numero: ");
    if (scanf("%d", &num1) != 1) {
        printf("Erro na leitura do primeiro numero.\n");
        return 1;
    }

    printf("Digite o segundo numero: ");
    if (scanf("%d", &num2) != 1) {
        printf("Erro na leitura do segundo numero.\n");
        return 1;
    }

    // Calcula a soma dos primos.
    resultado = soma_primos(num1, num2);

    // Imprime o resultado.
    printf("A soma dos numeros primos entre %d e %d (inclusive) e: %lld\n", num1, num2, resultado);

    return 0;
}

