/*
	Implementar uma função que, dado um número inteiro, calcule (e retorne) a soma 
	e a média de seus divisores.
*/

#include <stdio.h>
#include <stdlib.h> // Necessário para a função 'atoi'

// protótipos das funções
void somaMediaDivisores (int numero, int *soma, float *media);

// implementação da função main
int main() // Alterado para o padrão recomendado 'int main'
{
    // declaração de variáveis
    int n, s;
    float m;
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

    // Validação básica para o número
    if (n <= 0) {
        printf("A funcao e valida para numeros positivos.\n");
        // Pode-se optar por usar abs(n) ou terminar o programa.
        return 1;
    }

    // chamando a função, passando o número e os ENDEREÇOS (&) das variáveis s e m
    somaMediaDivisores (n, &s, &m);
    
    // exibindo os resultados
    printf ("\nSoma dos divisores de %d: %d\n", n, s);
    printf ("Media dos divisores de %d: %.1f\n", n, m);
    
    return 0;
}

// implementação das funções

/*Dado um número inteiro (numero), calcula a soma e a média de seus divisores.
  Entrada: numero
  Saída:   soma, media  (retornadas via ponteiros)  */
void somaMediaDivisores (int numero, int *soma, float *media)
{
    // declaração de variáveis
    int i, sum = 0, cont = 0;
    
    // variando todos os "possíveis" divisores de 'numero'
    // Como 'numero' é positivo (verificado na main), o loop funciona corretamente
    for (i = 1; i <= numero; i++)
    {
        // verificando se cada valor do intervalo é divisor de 'numero'
        if (numero % i == 0)
        {
            // sendo 'i' divisor de 'numero', acumulá-lo no somatório
            sum = sum + i; // ou sum += i;
            cont++;
        }
    }
    
    // armazenando os resultados nos parâmetros de saída (usando o operador *)
    // O valor do endereço *soma recebe o valor de sum
    *soma = sum;

    // Calculando e armazenando a média.
    // O valor do endereço *media recebe o valor da média.
    if (cont > 0) {
        // Conversão explícita de 'sum' para float antes da divisão para garantir o resultado correto.
        *media = (float)sum / cont;    
    } else {
        // Caso de erro (não deve ocorrer se numero >= 1)
        *media = 0.0f; 
    }
}
