#include <stdio.h>
#include <stdlib.h> // Necessário para a função 'atoi'

// declaração dos protótipos das funções
int mmc (int num1, int num2);
int calculaMaior (int a, int b);

// implementação da função main
int main() // Alterado para o padrão recomendado 'int main'
{
    // declaração de variáveis
    int valor1, valor2, resp;
    char buffer[20]; // Buffer para armazenar a entrada como string
    
    // --- 1. Leitura e conversão do primeiro valor (fgets + atoi) ---
    printf ("Entre com o primeiro valor: ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        valor1 = atoi(buffer);
    } 
    else 
    {
        printf("Erro na leitura do primeiro valor.\n");
        return 1; 
    }
    
    // --- 2. Leitura e conversão do segundo valor (fgets + atoi) ---
    printf ("\nEntre com o segundo valor: ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        valor2 = atoi(buffer);
    }
    else 
    {
        printf("Erro na leitura do segundo valor.\n");
        return 1;
    }
    
    // Validando entradas zero ou negativas para o cálculo de MMC (que assume inteiros positivos)
    if (valor1 <= 0 || valor2 <= 0) {
        printf("\nO calculo do MMC requer numeros inteiros positivos. Usando valores absolutos maiores que zero.\n");
        valor1 = abs(valor1);
        valor2 = abs(valor2);
        
        // Se um dos números for 0, o MMC é indefinido ou 0 dependendo da convenção.
        // Como a lógica do loop em mmc() falha se um for 0, saímos aqui.
        if (valor1 == 0 || valor2 == 0) {
            printf("MMC nao aplicavel com zero.\n");
            return 1;
        }
    }
    
    // chamando a função
    resp = mmc(valor1, valor2);
    
    // exibindo o resultado
    printf ("MMC (%d, %d) = %d\n", valor1, valor2, resp);

    return 0;
}

// implementação das funções

// Função auxiliar para determinar o maior número
int calculaMaior (int a, int b)
{
    return (a > b) ? a : b;
}

// Função para calcular o MMC
int mmc (int num1, int num2)
{
    // declaração de variáveis
    int maior, i, prod;
    
    // determinando o maior valor entre num1 e num2
    maior = calculaMaior (num1, num2);

    // O loop para no máximo no produto dos números (o MMC nunca o excede)
    prod = num1 * num2; 
    
    // variando de 'maior' até o produto entre eles, em busca do MMC
    for (i = maior; i <= prod; i++)
    {
        // verificando se 'i' é um múltiplo comum de 'num1' e 'num2'
        if ((i % num1 == 0) && (i % num2 == 0))
        {
            // O primeiro múltiplo comum encontrado é o MMC
            return i; 
        }    
    }
    
    // Deve retornar 0 se algo der errado (o loop não deve falhar se os números forem positivos)
    return 0;
}
