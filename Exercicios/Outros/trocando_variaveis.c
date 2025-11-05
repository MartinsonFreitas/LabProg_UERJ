#include <stdio.h>
#include <stdlib.h> 

// protótipos/cabeçalhos das funções
void trocar (int *a, int *b);

// implementação da função main
int main () 
{
    // declaração de variáveis
    int valor1, valor2;
    char buffer[20];
    
    // --- Leitura e conversão (fgets + atoi) ---
    printf ("Entre com o primeiro numero: ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        valor1 = atoi(buffer);
    } 
    else 
    {
        return 1; 
    }
    
    printf ("Entre com o segundo numero: ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        valor2 = atoi(buffer);
    }
    else 
    {
        return 1;
    }
    
    // 1. Antes da Chamada
    printf ("\nAntes da chamada: valor1 = %d e valor2 = %d\n", valor1, valor2);
    
    // 2. Chamada - PASSANDO ENDEREÇOS (&)
    // Se o usuário digitou 10 e 20, a função recebe &10 e &20.
    trocar (&valor1, &valor2);
    
    // 3. Depois da Chamada - VALORES TROCADOS
    printf ("Depois da chamada: valor1 = %d e valor2 = %d\n", valor1, valor2);  
    
    return 0;
}

// implementação das demais funções
void trocar (int *a, int *b)
{
    // CÓDIGO CRÍTICO PARA TROCA COM PONTEIROS
    int c;
    
    // 1. Guarda o valor de *a na variável temporária c
    c = *a; 
    
    // 2. Coloca o valor de *b no endereço de *a
    *a = *b; 
    
    // 3. Coloca o valor temporário (original de *a) no endereço de *b
    *b = c;
}
