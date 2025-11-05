/*
Desenvolver uma função que exiba todos os números primos inferiores a 
	determinado valor inteiro N.
*/

#include <stdio.h>
#include <stdlib.h> // Necessário para a função 'atoi'

// protótipos das funções
void primo (int n);

// implementação da função main
int main() // Alterado para o padrão recomendado 'int main'
{
    // declaração de variáveis
    int n;
    char buffer[20]; // Buffer para armazenar a entrada como string
    
    // --- Leitura e conversão do valor de entrada (fgets + atoi) ---
    printf("Digite um numero: ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        // Converte a string lida para inteiro
        n = atoi(buffer);
    } 
    else 
    {
        printf("Erro na leitura do numero.\n");
        return 1; 
    }
    
    // Chamando a função
    printf("\nNumeros classificados como primos menores que %d: ", n);
    primo(n);
    printf("\n"); // Adiciona uma nova linha para formatar a saída
    
    return 0;
}

// implementação das funções
void primo (int n)
{
    // declaração de variáveis
    int i, j, cont;

    // variando os valores no intervalo de 1 a n-1 (ou seja, números inferiores a n)
    // Nota: O número 1 é contado, mas o código apenas exibirá números com cont==2,
    // o que funciona para excluir o 1 (que tem cont==1).
    for (i = 1; i < n; i++)
    {
        // zerando a quantidade de divisores do número do intervalo (i)
        cont = 0;

        // verificando quantos divisores o número 'i' possui
        // O loop pode ir até a raiz quadrada de 'i' para ser mais eficiente,
        // mas mantemos a lógica original para contagem de divisores.
        for(j = 1; j <= i; j++)
        {
            // verificando se 'j' é divisor de 'i'
            if (i % j == 0)
            {
                cont++;
            }
        }
        
        // verificando se 'i' é primo (ou seja, se possui 2 divisores: 1 e ele mesmo)
        if (cont == 2)
        {
            printf("%d ", i);
        }
    }
}
