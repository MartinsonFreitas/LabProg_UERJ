/*
	Implementar uma função que, dado um número inteiro n, e o intervalo definido 
	pelos números a e b, determine quantos valores do intervalo possuem 
	divisores (sendo estes diferentes de 1) que também sejam divisores de n.

	Exemplo:
		n = 6
		a = 10 e b = 20

		divisores de 6? 1, 2, 3, 6

		10 - 2, 5, 10 (já descartando o 1)		*
		11 - 11
		12 - 2, 3, 4, 6, 12						*
		13 - 13
		14 - 2, 7, 14							*
		15 - 3, 5, 15							*
		16 - 2, 4, 8, 16						*
		17 - 17
		18 - 2, 3, 6, 9, 18						*
		19 - 19
		20 - 2, 4, 5, 10, 20					*
		
		Resposta = 7 (pois os números 10, 12, 14, 15, 16, 18 e 20 possuem divisores 
		              em comum com o número 6).
*/

#include <stdio.h>
#include <stdlib.h> // Necessário para a função 'atoi'

// protótipos das funções
int verificaDivisores (int n, int a, int b);

// implementação da função main
int main () // Alterado para o padrão recomendado 'int main'
{
    // declaração de variáveis
    int numero, limI, limS, resp;
    char buffer[20]; // Buffer para armazenar a entrada como string
    
    // --- 1. Leitura e conversão do 'numero' (fgets + atoi) ---
    printf ("Entre com o numero: ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        numero = atoi(buffer);
    } 
    else 
    {
        printf("Erro na leitura do numero.\n");
        return 1; 
    }
    
    // --- 2. Leitura e conversão do 'limI' (fgets + atoi) ---
    printf ("Entre com o limite INFERIOR do intervalo: ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        limI = atoi(buffer);
    } 
    else 
    {
        printf("Erro na leitura do limite inferior.\n");
        return 1;
    }

    // --- 3. Leitura e conversão do 'limS' (fgets + atoi) ---
    printf ("Entre com o limite SUPERIOR do intervalo: ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        limS = atoi(buffer);
    }
    else 
    {
        printf("Erro na leitura do limite superior.\n");
        return 1;
    }
    
    // Ajustando a ordem do intervalo para a função (se necessário)
    int inicio = (limI < limS) ? limI : limS;
    int fim = (limI < limS) ? limS : limI;
    
    // chamando a função
    resp = verificaDivisores (numero, inicio, fim);
    
    // exibindo o resultado
    printf ("\nExistem %d valores no intervalo [%d, %d] com divisores em comum com %d.\n", resp, inicio, fim, numero);

    return 0;
}

// implementação das funções
int verificaDivisores (int n, int a, int b)
{
    // declaração de variáveis
    int i, j, cont = 0;
    
    // Garantindo que 'n' seja positivo para o loop de divisores.
    if (n <= 1) {
        // Se n for 0 ou 1, não há divisores comuns maiores que 1.
        return 0; 
    }
    
    // variando os valores do intervalo
    for (i = a; i <= b; i++)
    {
        // Se o número atual 'i' for 0, pula (evita divisão por zero se j for 0, embora j comece em 2)
        if (i == 0) continue; 
        
        // Loop para verificar se existe ALGUM divisor comum (j >= 2)
        // Se encontrarmos um divisor, podemos parar o loop interno
        int encontrouDivisor = 0;

        // verificando divisores de 'n' (de 2 até n)
        for (j = 2; j <= n; j++)
        {
            if (n % j == 0) { // j é divisor de 'n'
                if (i % j == 0) { // j também é divisor de 'i'
                    encontrouDivisor = 1;
                    break; // Encontrou um, pode sair do loop de 'j' e contar 'i'
                }
            }
        }
        
        // verificando se existem divisores comuns entre 'i' e 'n'
        if (encontrouDivisor)
        {
            cont++;
        }
    }
    
    // retornando resultado
    return cont;
}
