#include <stdio.h>

// 1. Função para verificar se um número é primo (mantida por ser eficiente)
// Retorna 1 (verdadeiro) se for primo, 0 (falso) caso contrário.
int eh_primo(int num) {
    if (num <= 1) {
        return 0;
    }
    if (num == 2) {
        return 1;
    }
    if (num % 2 == 0) {
        return 0;
    }

    // Verifica divisibilidade de 3 até a raiz quadrada de 'num', pulando os pares.
    for (int i = 3; i * i <= num; i = i + 2) {
        if (num % i == 0) {
            return 0; // Não é primo
        }
    }
    return 1; // É primo
}

// 2. Função auxiliar para ler e validar os números do usuário
// Armazena os valores lidos e ordenados em um array de 2 posições
// Retorna 1 se a leitura foi bem-sucedida, 0 caso contrário.
int ler_limites(int limites[]) {
    int num1, num2;

    printf("Digite o primeiro numero inteiro (limite inferior ou superior): ");
    if (scanf("%d", &num1) != 1) {
        printf("Erro na leitura do primeiro numero.\n");
        return 0;
    }

    printf("Digite o segundo numero inteiro (limite inferior ou superior): ");
    if (scanf("%d", &num2) != 1) {
        printf("Erro na leitura do segundo numero.\n");
        return 0;
    }

    // Ordena os números e armazena no array 'limites'
    if (num1 <= num2) {
        limites[0] = num1; // Início
        limites[1] = num2; // Fim
    } else {
        limites[0] = num2; // Início
        limites[1] = num1; // Fim
    }

    // Garante que o ponto inicial não seja menor que 2
    if (limites[0] < 2) {
        limites[0] = 2;
    }

    return 1;
}

// 3. Função para calcular e retornar a soma dos números primos
int calcular_soma_primos(int inicio, int fim) {
    int soma_primos = 0;

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
    printf("\n"); // Quebra de linha após a lista de primos

    return soma_primos;
}

// 4. Função principal (main) - Agora limpa e apenas coordenadora
int main() {
    // A única variável local na main é o array para os limites,
    // que é preenchido pela função 'ler_limites' (sem usar scanf diretamente)
    int limites[2]; // limites[0] = inicio, limites[1] = fim
    int resultado_soma;

    // A main chama a função de leitura
    if (!ler_limites(limites)) {
        // Se a leitura falhar, encerra o programa
        return 1;
    }

    // A main chama a função de cálculo, passando os limites lidos
    resultado_soma = calcular_soma_primos(limites[0], limites[1]);

    // A main exibe o resultado
    printf("\nA soma de todos os numeros primos no intervalo e: %d\n", resultado_soma);

    return 0;
}
