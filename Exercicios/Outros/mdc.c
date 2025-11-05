#include <stdio.h>
#include <stdlib.h> // Necessário para a função 'atoi'

// declaração dos protótipos das funções
int mdc (int num1, int num2);

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
    
    // Garantindo que os valores sejam positivos para o cálculo do MDC
    if (valor1 < 0 || valor2 < 0) {
        printf("\nO calculo do MDC e tipicamente feito para numeros inteiros positivos.\n");
        // Convertendo para valor absoluto para continuar
        valor1 = abs(valor1); 
        valor2 = abs(valor2);
    }
    
    // chamando a função
    resp = mdc(valor1, valor2);
    
    // exibindo o resultado
    printf ("MDC (%d, %d) = %d\n", valor1, valor2, resp);

    return 0;
}

// implementação das funções
int mdc (int num1, int num2)
{
    // declaração de variáveis
    int menor, i, result = 1; // Inicializa result com 1, pois 1 é o menor divisor comum.
    
    // determinando o menor valor entre num1 e num2
    if (num1 < num2)
    {
        menor = num1;
    }
    else
    {
        menor = num2;
    }
    
    // Se um dos números for 0, o MDC é o outro número (ou 0 se ambos forem 0)
    if (num1 == 0 && num2 == 0) return 0;
    if (num1 == 0) return num2;
    if (num2 == 0) return num1;


    // variando de 1 até o menor, em busca do MDC
    for (i = 1; i <= menor; i++)
    {
        // verificando se 'i' é um divisor comum de 'num1' e 'num2'
        if ((num1 % i == 0) && (num2 % i == 0))
        {
            // O último divisor comum encontrado será o MDC
            result = i;
        }    
    }
    
    // retornando o resultado
    return result;    
}
