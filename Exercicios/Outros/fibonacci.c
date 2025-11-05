#include <stdio.h>
#include <stdlib.h> // Necessário para a função 'atoi'

// declaração dos protótipos das funções
int fibonacci (int n);

// implementação da função main
int main() // Alterado para o padrão recomendado 'int main'
{
    // declaração de variáveis
    int posicao;
    char buffer[20]; // Buffer para armazenar a entrada como string
    
    // leitura do elemento que o usuário deseja saber
    printf ("Qual a posicao ordinal da sequencia de fibonacci? ");
    
    // --- Leitura e conversão da posição (fgets + atoi) ---
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        // Converte a string lida para inteiro
        posicao = atoi(buffer);
    } 
    else 
    {
        printf("Erro na leitura da posicao.\n");
        return 1; 
    }
    
    // Validação básica para evitar números negativos
    if (posicao <= 0) {
        printf("A posicao deve ser um numero inteiro positivo.\n");
        return 1;
    }
    
    // chamando a função e exibindo o resultado
    printf ("%do. elemento de Fibonacci: %d\n", posicao, fibonacci(posicao)); 
    
    return 0;
}

// implementação das funções
int fibonacci (int n)
{
    // declaração de variáveis
    int i, t1, t2, resposta;
    
    // A lógica original usa 1 e 2 como base.
    // Sequência: 1, 1, 2, 3, 5, 8, ...
    
    // se for o 1º termo, retorna 1
    if (n == 1)
    {
        resposta = 1;
    }
    // se for o 2º termo, retorna 1
    else if (n == 2)
    {
        resposta = 1;
    }
    else
    {
        // se chegou aqui, é pq deseja-se calcular um valor que está da terceira posição em diante
        t1 = 1; // Termo anterior (n-2)
        t2 = 1; // Termo atual (n-1)

        // calculando do 3º ao n-ésimo termo
        for (i = 3; i <= n; i++)
        {
            // O próximo termo é a soma dos dois anteriores
            resposta = t1 + t2;
            
            // Atualiza t1 e t2 para o próximo loop
            t1 = t2;          // t1 recebe o antigo t2
            t2 = resposta;    // t2 recebe a nova resposta
        }
    }
    
    // retornando o resultado
    return resposta;    
}
