#include <stdio.h>
#include <stdlib.h> // Necessário para a função 'atoi'

// protótipos/cabeçalhos das funções
void exibeNumeros ();

// implementação da função main
int main () // Padrão recomendado para a função main
{
    exibeNumeros ();
    return 0; // Adiciona o retorno 0
}

// implementação das demais funções
void exibeNumeros ()
{
    // declaração de variáveis
    int i, num1, num2;
    // Buffer para armazenar a entrada como string (tamanho suficiente para um inteiro e o '\n')
    char buffer[20]; 
    
    // 1. Leitura e conversão do valor de 'num1'
    printf ("Entre com o primeiro valor do intervalo: ");
    
    // Lê a linha inteira da entrada padrão (stdin)
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        // Converte a string lida (ex: "10\n\0") para inteiro (10)
        num1 = atoi(buffer);
    } 
    else 
    {
        // Em caso de erro na leitura
        printf("Erro na leitura do primeiro número.\n");
        return; 
    }
    
    // 2. Leitura e conversão do valor de 'num2'
    printf ("Entre com o segundo valor do intervalo: ");
    
    // Lê a linha inteira da entrada padrão (stdin)
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        // Converte a string lida para inteiro
        num2 = atoi(buffer);
    }
    else 
    {
        // Em caso de erro na leitura
        printf("Erro na leitura do segundo número.\n");
        return;
    }
    
    // 3. Verifica e ajusta a ordem para garantir que a contagem seja ascendente
    if (num1 > num2) {
        printf("O primeiro valor é maior que o segundo. Trocando os valores.\n");
        int temp = num1;
        num1 = num2;
        num2 = temp;
    }
    
    printf("Exibindo números de %d a %d:\n", num1, num2);
    // variando os valores de 'num1' a 'num2' e exibindo-os
    for (i = num1; i <= num2; i++)
    {
        printf ("%d ", i);
    }
    printf("\n"); // Adiciona uma nova linha para melhor visualização
}
