/*
	Implementar uma função que, dado um número inteiro N, exiba uma sequência de 
	números como as ilustradas nos exemplos a seguir:

	Exemplo 1:  N = 3
 	(1 2) (1 3) (2 1) (2 3) (3 1) (3 2)

	Exemplo 2:  N = 5
 	(1 2) (1 3) (1 4) (1 5) (2 1) (2 3) (2 4) (2 5) (3 1) (3 2) (3 4) (3 5) (4 1) 
	(4 2) (4 3) (4 5) (5 1) (5 2) (5 3) (5 4)
*/

#include <stdio.h>
#include <stdlib.h> // Necessário para a função 'atoi'

// protótipos das funções
void sequencia(int n);

// implementação da função main
int main () // Alterado para o padrão recomendado 'int main'
{
    // declaração de variáveis
    int n;
    char buffer[20]; // Buffer para armazenar a entrada como string
    
    // --- Leitura e conversão do valor de n (fgets + atoi) ---
    printf ("Entre com um valor (n): ");
    
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        // Converte a string lida para inteiro
        n = atoi(buffer);
    } 
    else 
    {
        printf("Erro na leitura do valor.\n");
        return 1; 
    }
    
    printf ("\n");
    
    // Validação básica para n (deve ser positivo)
    if (n <= 0) {
        printf("O valor de n deve ser positivo.\n");
        return 1;
    }
    
    // chamando a função
    sequencia (n);

    return 0;
}

// implementação das funções
void sequencia(int n)
{
    // declaração de variáveis 
    int i, j;
    
    
    for(i = 1; i <= n; i++)
    {
        for(j = 1; j <= n; j++)
        {   
            // Apenas exibe o par se i for diferente de j
            if(j != i)
            {
                printf("(%d %d) ", i, j);    
            }
        }
        printf ("\n"); // Nova linha após cada iteração do loop 'i'
    }
}
