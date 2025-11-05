#include <stdio.h>
#include <stdlib.h> // Necessário para a função 'atoi'

// protótipos/cabeçalhos das funções (opcional, mas boa prática)
void exibaMultiplos (int n, int a, int b);

// implementação das funções
void exibaMultiplos (int n, int a, int b)
{
    // declaração de variáveis
    int i;
    
    // Garantindo que o intervalo seja percorrido da menor para a maior
    int inicio = (a < b) ? a : b;
    int fim = (a < b) ? b : a;

    printf("\n--- Múltiplos de %d no intervalo [%d, %d] ---\n", n, inicio, fim);

    // Se o número para múltiplos for 0, evitamos a divisão por zero ou um loop inútil
    if (n == 0) {
        printf("Não é possível encontrar múltiplos de 0.\n");
        return;
    }
    
    // variando os números no intervalo de 'a' a 'b' (ajustado)
    for (i = inicio; i <= fim; i++)
    {
        // verificando se 'i' é múltiplo de 'n'
        if (i % n == 0)
        {
            printf ("%d ", i);
        }
    }
    printf("\n"); // Adiciona uma nova linha no final
}

// implementação da função main
int main () // Alterado para o padrão recomendado 'int main'
{
    // declaração de variáveis para os valores de entrada
    int n, a, b;
    char buffer[20]; // Buffer para ler a entrada
    
    // --- 1. Leitura e conversão de N (o número base) ---
    printf ("Entre com o numero base (N) para os multiplos: ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        n = atoi(buffer);
    } 
    else 
    {
        printf("Erro na leitura de N.\n");
        return 1; 
    }

    // --- 2. Leitura e conversão de A (início do intervalo) ---
    printf ("Entre com o inicio do intervalo (A): ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        a = atoi(buffer);
    } 
    else 
    {
        printf("Erro na leitura de A.\n");
        return 1;
    }

    // --- 3. Leitura e conversão de B (fim do intervalo) ---
    printf ("Entre com o fim do intervalo (B): ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        b = atoi(buffer);
    }
    else 
    {
        printf("Erro na leitura de B.\n");
        return 1;
    }
    
    // chamada da função com os valores fornecidos pelo usuário
    exibaMultiplos (n, a, b);
    
    return 0;
}
