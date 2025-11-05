/* 
 	Implementar uma função que, dado um vetor de reais, troque o 1º e o 2º elementos, 
	em seguida o 3º e o 4º elementos e assim sucessivamente, até se chegar ao final 
	do vetor.
	
	Exemplos:
	
	suponha o seguinte vetor com 10 elementos
		vet: 1  2  3  4  5  6  7  8  9  0
		vet: 2  1  4  3  6  5  8  7  0  9

	suponha o seguinte vetor com 5 elementos
		vet: 1  2  3  4  5 
		vet: 2  1  4  3  5
*/

#include <stdio.h>
#include <stdlib.h> // Necessário para a função 'atof'

// definição de constantes
#define TAMANHO 11

// protótipos das funções
void preencheVetor (float vetor[], int quant);
void exibirVetor (float vetor[], int quant);
void trocar (float vetor[], int quant);

// implementação da função main
int main() // Alterado para o padrão recomendado 'int main'
{
    // declaração de variáveis
    float vet[TAMANHO];

    // preenchendo o vetor com valores digitados pelo usuário (fgets + atof)
    preencheVetor (vet, TAMANHO);
    
    // antes da troca
    printf ("\nAntes da troca:\n");
    exibirVetor (vet, TAMANHO);
    
    // chamando a função de troca
    trocar (vet, TAMANHO);

    // depois da troca
    printf ("\nDepois da troca:\n");
    exibirVetor (vet, TAMANHO);

    return 0;
}

// implementação das funções

// Função para preencher o vetor com valores digitados pelo usuário (usando fgets + atof)
void preencheVetor (float vetor[], int quant)
{
    // declaração de variáveis
    int i;
    char buffer[20]; // Buffer para ler a string de entrada
    
    // percorrendo o vetor
    for (i = 0; i < quant; i++)
    {
        printf ("Entre com o valor para a posicao [%d]: ", i);
        
        // Leitura da string (fgets)
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
        {
            // Conversão para float (atof)
            vetor[i] = atof(buffer);
        } else {
            // Em caso de erro, atribui 0.0 e avisa
            vetor[i] = 0.0f;
            printf("Erro de leitura, valor padrao 0.0 atribuido.\n");
        }
    }
}

// Função para exibir o vetor
void exibirVetor (float vetor[], int quant)
{
    // declaração de variáveis
    int i;
    
    // exibindo os elementos do vetor
    printf ("\nElementos do vetor: ");
    
    for (i = 0; i < quant; i++)
    {
        printf ("%.1f ", vetor[i]);
    }
    
    printf ("\n\n");
}

// Função para trocar pares de elementos
void trocar (float vetor[], int quant)
{
    // declaração de variáveis
    int i, j;
    float aux;
    
    // i: 0, 2, 4, 6, 8, 10
    // j: 1, 3, 5, 7, 9
    // O loop para quando j alcançar quant (11) ou mais.
    for (i = 0, j = 1; j < quant; i += 2, j += 2)
    {
        // trocando os valores que estão nas posições i e j
        aux = vetor[i];
        vetor[i] = vetor[j];
        vetor[j] = aux;
    }
}
