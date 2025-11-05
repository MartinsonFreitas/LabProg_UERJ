#include <stdio.h>
#include <stdlib.h> // Necessário para a função 'atof'

// declaração dos protótipos das funções
float converte (float tempC);

// implementação da função main
int main() // Alterado para o padrão recomendado 'int main'
{
    float tempC, novaTemp;
    char buffer[20]; // Buffer para armazenar a entrada como string
    
    printf("ENTRE COM A TEMPERATURA (C): ");
    
    // --- Leitura e conversão da temperatura (fgets + atof) ---
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        // Converte a string lida (incluindo o ponto decimal) para float
        tempC = atof(buffer);
    } 
    else 
    {
        printf("Erro na leitura da temperatura.\n");
        return 1; 
    }

    novaTemp = converte(tempC);
    
    // Exibe o resultado com uma casa decimal (%.1f)
    printf("%.1f Celsius para:\n%.1f Fahrenheit\n", tempC, novaTemp);
    
    return 0;
}

// implementação das funções
float converte(float tempC)
{
    float tempF;
    
    // Fórmula de conversão: F = (C * 9/5) + 32
    // Nota: É uma boa prática usar 9.0/5.0 para garantir que a divisão seja feita em ponto flutuante.
    tempF = (tempC * 9.0 / 5.0) + 32.0;

    return tempF;
}
