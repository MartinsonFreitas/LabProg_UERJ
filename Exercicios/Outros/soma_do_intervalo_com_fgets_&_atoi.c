#include <stdio.h>
#include <stdlib.h> // Necessário para a função 'atoi'

// protótipos/cabeçalhos das funções
int somaNumeros (int num1, int num2);

// implementação da função main
int main () // Alterado para o padrão recomendado 'int main'
{
    // declaração de variáveis
    int valor1, valor2, resultado;
    char buffer[20]; // Buffer para armazenar a entrada como string
    
    printf ("\n\n\nEntre com um numero: ");
    
    // 1. Leitura e conversão do 'valor1'
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        // Converte a string lida para inteiro
        valor1 = atoi(buffer);
    } 
    else 
    {
        printf("Erro na leitura do primeiro número.\n");
        return 1; // Retorna um código de erro
    }
    
    printf ("Entre com outro numero: ");
    
    // 2. Leitura e conversão do 'valor2'
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        // Converte a string lida para inteiro
        valor2 = atoi(buffer);
    }
    else 
    {
        printf("Erro na leitura do segundo número.\n");
        return 1; // Retorna um código de erro
    }
    
    // Verifica e ajusta a ordem dos valores, se necessário, para garantir a soma correta
    int num_inicio = (valor1 < valor2) ? valor1 : valor2;
    int num_fim = (valor1 < valor2) ? valor2 : valor1;
    
    if (valor1 != num_inicio) {
        printf("A ordem dos números foi ajustada para o cálculo do intervalo.\n");
    }
    
    // chamando a função com os valores ajustados (ou originais, se já estavam em ordem)
    resultado = somaNumeros (num_inicio, num_fim);
    
    // exibe o resultado
    printf ("\n\nSoma dos números no intervalo [%d, %d] = %d\n", num_inicio, num_fim, resultado);
    
    return 0;
}

// implementação das demais funções
int somaNumeros (int num1, int num2)
{
    // declaração de variáveis
    long long soma = 0; // Usando 'long long' para evitar overflow em somas grandes
    int i;
    
    // variando os valores de 'num1' a 'num2' e somando-os
    // O ajuste da ordem foi movido para 'main', então assumimos que num1 <= num2 aqui.
    for (i = num1; i <= num2; i++)
    {
        soma = soma + i;    // soma += i;
    }
    
    // retornar o resultado. O casting para int pode ser necessário se a soma for muito grande.
    // Como a variável 'resultado' em main é 'int', faremos o casting se necessário,
    // mas retornamos o long long e esperamos que caiba em 'int'.
    return (int)soma;
}
