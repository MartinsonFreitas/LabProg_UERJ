#include <stdio.h>
#include <stdlib.h> // Incluído para usar a função abs() na ordenação

// 1. Função para verificar se um número é primo
// Retorna 1 (verdadeiro) se for primo, 0 (falso) caso contrário.
int eh_primo(int num) {
    // Números menores ou iguais a 1 não são primos
    if (num <= 1) {
        return 0;
    }

    // O número 2 é o único primo par
    if (num == 2) {
        return 1;
    }

    // Se for par e maior que 2, não é primo
    if (num % 2 == 0) {
        return 0;
    }

    // Verifica divisibilidade de 3 até a raiz quadrada de 'num',
    // pulando os pares (incremento de 2).
    // O loop começa em 3 e i*i <= num é o otimizador comum.
    for (int i = 3; i * i <= num; i = i + 2) {
        if (num % i == 0) {
            return 0; // Encontrou um divisor, não é primo
        }
    }

    // Se não encontrou divisores, é primo
    return 1;
}

// 2. Função principal (main)
int main() {
    int num1, num2;
    int inicio, fim;
    long long soma_primos = 0; // Usa long long para evitar overflow se a soma for grande

    // Leitura dos 2 números
    printf("Digite o primeiro numero inteiro (limite inferior ou superior): ");
    if (scanf("%d", &num1) != 1) {
        printf("Erro na leitura do primeiro numero.\n");
        return 1; // Retorna código de erro
    }

    printf("Digite o segundo numero inteiro (limite inferior ou superior): ");
    if (scanf("%d", &num2) != 1) {
        printf("Erro na leitura do segundo numero.\n");
        return 1; // Retorna código de erro
    }

    // Determina o início e o fim do intervalo, garantindo que inicio <= fim.
    // Isso torna o programa robusto caso o usuário digite os números fora de ordem.
    if (num1 <= num2) {
        inicio = num1;
        fim = num2;
    } else {
        inicio = num2;
        fim = num1;
    }

    // A questão especifica que o intervalo deve ser "entre os 2, inclusive".
    // E, implicitamente, que o menor número primo é 2.
    // Se 'inicio' for menor que 2, o loop deve começar em 2.
    if (inicio < 2) {
        inicio = 2;
    }

    printf("\n--- Calculo da Soma dos Primos ---\n");
    printf("Intervalo de busca (inclusive): %d a %d\n", inicio, fim);
    printf("Numeros primos encontrados: ");

    // Itera por todos os números no intervalo [inicio, fim]
    for (int i = inicio; i <= fim; i++) {
        if (eh_primo(i)) {
            soma_primos += i;
            printf("%d ", i);
        }
    }

    // Exibe o resultado
    printf("\n\nA soma de todos os numeros primos no intervalo e: %lld\n", soma_primos);

    return 0;
}
