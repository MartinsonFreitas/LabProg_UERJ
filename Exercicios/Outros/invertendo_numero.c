#include <stdio.h>
#include <stdlib.h> // Necessário para a função 'atoi'

// protótipos/cabeçalhos das funções
int inverte (int num);

// implementação da função main
int main ()
{
    // declaração de variáveis
    int num, numInvertido;
    char buffer[20]; // Buffer para armazenar a entrada como string
    
    // --- Leitura e conversão do número (fgets + atoi) ---
    printf ("Entre com um numero de ate 3 digitos: ");
    
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) 
    {
        // Converte a string lida para inteiro
        num = atoi(buffer);
    } 
    else 
    {
        printf("Erro na leitura do número.\n");
        return 1; 
    }
    
    // Validando para garantir que o número tenha no máximo 3 dígitos, se necessário
    if (num < 0 || num > 999) 
    {
        printf("Numero fora do intervalo de 0 a 999. Terminando...\n");
        return 1;
    }
    
    // chamando a função
    numInvertido = inverte (num);
    
    // exibe o resultado
    printf ("\nO numero %d invertido eh: %d\n", num, numInvertido);
    
    return 0;
}

// implementação da função para inverter um número de até 3 dígitos
int inverte (int num)
{
    // declaração de variáveis
    int unidade, dezena, centena, numInvertido;
    
    // 1. Obter a unidade (último dígito)
    unidade = num % 10;
    
    // 2. Obter a dezena (segundo dígito)
    // Primeiro remove a unidade, depois pega o resto por 10
    dezena = (num / 10) % 10;
    
    // 3. Obter a centena (primeiro dígito)
    // Divide por 100
    centena = num / 100;
    
    // 4. Montar o número invertido
    // A unidade vira centena (x 100)
    // A dezena permanece dezena (x 10)
    // A centena vira unidade (x 1)
    numInvertido = (unidade * 100) + (dezena * 10) + centena;
    
    // retornar o resultado
    return numInvertido;
}
