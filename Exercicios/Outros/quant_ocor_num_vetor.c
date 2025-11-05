/* 
 * 	Desenvolver uma função que determine o número de ocorrências de um número 
	inteiro x em um vetor A.
*/

#include <stdio.h>
#include <stdlib.h> // Necessário para a função 'atoi'

// definição de constantes
#define TAMANHO 5

// protótipos das funções
int numOcorrencias (int numero, int vetor[TAMANHO]);
void preencheVetor (int vetor[TAMANHO]);

// implementação da função main
int main() // Alterado para o padrão recomendado 'int main'
{
    // declaração de variáveis
    int vet[TAMANHO];
    int resultado, valor;
    char buffer[20]; // Buffer para armazenar a entrada como string
    
    // preenchendo o vetor
    preencheVetor (vet);
    
    // --- Leitura e conversão do valor a ser buscado (fgets + atoi) ---
    printf ("\n\nQual valor deseja buscar no vetor? ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        // Converte a string lida para inteiro
        valor = atoi(buffer);
    } 
    else 
    {
        printf("Erro na leitura do valor de busca.\n");
        return 1; 
    }
    
    // chamando a função
    resultado = numOcorrencias (valor, vet);
    
    // exibindo o resultado
    printf ("Foram encontradas %d ocorrencias do numero %d no vetor.\n", resultado, valor);

    return 0;
}

// implementação das funções

// Função para contar ocorrências
int numOcorrencias (int numero, int vetor[TAMANHO])
{
    // declaração de variáveis
    int i, cont = 0;
    
    // percorrer todas as posições do vetor
    for (i = 0; i < TAMANHO; i++)
    {
        // comparar o numero sendo buscado com cada valor do vetor
        if (numero == vetor[i])
        {
            cont++;
        }
    }
    
    // retornando o resultado
    return cont;
}

// Função para preencher o vetor, substituindo scanf por fgets + atoi
void preencheVetor (int vetor[TAMANHO])
{
    // declaração de variáveis
    int i;
    char buffer[20]; // Buffer LOCAL para a função
    
    // percorrendo o vetor
    for (i = 0; i < TAMANHO; i++)
    {
        printf ("Entre com o %do numero: ", i + 1);
        
        // Uso de fgets + atoi para ler CADA elemento
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
        {
            vetor[i] = atoi(buffer);
        } else {
            // Em caso de erro na leitura, atribui 0 e avisa
            vetor[i] = 0;
            printf("Erro de leitura, valor padrao 0 atribuido.\n");
        }
    }
}
