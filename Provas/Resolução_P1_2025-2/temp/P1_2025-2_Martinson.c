#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// tamanho máximo da mensagem
#define MAX_MESSAGE 101 // 100 caracteres + \0

//definição de constantes
#define TRUE 1
#define FALSE 0

int isPrime(int n);
int sumOfSquareDigits(int n);
int isHappy(int n);
void encryptCaesar(char *message, int k);

// Função main
int main() {
    
    // Questão 1: Verifica se eh primo...
    int num;
    char numero[20]; // Buffer para armazenar a entrada como string
    
    // --- Leitura e conversão do valor de entrada (fgets + atoi) ---
    printf("Digite um numero: ");
    if (fgets(numero, sizeof(numero), stdin) != NULL) 
    {
        // Converte a string lida para inteiro
        num = atoi(numero);
    } 
    else 
    {
        printf("Erro na leitura do numero.\n");
    }
    
    // Verificação de Primo Feliz
    int is_prime = isPrime(num);
    int is_happy = isHappy(num);
    int is_happy_prime = is_prime && is_happy;
    
    if (!is_happy_prime) {
        printf("Não é primo feliz. Fim do programa.\n");
        return 0; // Encerra se não for Primo Feliz
    }
    
    printf("\n----------------------------------------------\n\n");
    
    /////////////////////////
    
    // Limpa o buffer de entrada após o scanf
    //while (getchar() != '\n');
    
    // Leitura do Número-Chave N
    //int N;
    char buffer[MAX_MESSAGE];
    
    /*
    //printf("--- Questão 2: Criptografia dos Primos Felizes ---\n");
    printf("a. Digite o Número-Chave N [1, 10000]: ");
    if (scanf("%d", &N) != 1) {
        fprintf(stderr, "Erro na leitura do Número-Chave.\n");
        return 1;
    }
    */

    // Limpa o buffer de entrada após o scanf para a leitura da string
    //while (getchar() != '\n');


    
    
    // Criação da Chave de Criptografia K
    int K = num % 26;
    
    // Leitura da mensagem de texto
    printf("b. N é primo feliz! Chave K = %d\n", K);
    printf("c. Digite a mensagem (apenas maiúsculas, máx. 100 caracteres): ");
    
    // Leitura segura da string
    if (fgets(buffer, MAX_MESSAGE, stdin) == NULL) {
        fprintf(stderr, "Erro na leitura da mensagem.\n");
        return 1;
    }
    
    // Remove o caractere de nova linha '\n' adicionado pelo fgets, se existir
    buffer[strcspn(buffer, "\n")] = 0;
    
    // 2.d. Cifra de César e impressão
    encryptCaesar(buffer, K);
    
    printf("Mensagem Criptografada: %s\n", buffer);

    printf("----------------------------------------------\n");
    
    return 0;
}


// verifica se eh primo
int isPrime(int n) {
    if (n <= 1) {
        return FALSE; // Números menores ou iguais a 1 não são primos.
    }
    if (n <= 3) {
        return TRUE; // 2 e 3 são primos.
    }
    // Otimização: Ignora múltiplos de 2 e 3, verificando a partir de 5.
    if (n % 2 == 0 || n % 3 == 0) {
        return FALSE;
    }
    
    // Verifica divisores da forma 6k ± 1 até a raiz quadrada de n.
    for (int i = 5; (long long)i * i <= n; i = i + 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return FALSE;
        }
    }
    return TRUE;
}

// soma dos quadrados dos digitos
int sumOfSquareDigits(int n) {
    int sum = 0;
    int digit;
    while (n > 0) {
        digit = n % 10;
        sum += digit * digit;
        n /= 10;
    }
    return sum;
}

// verifica se eh feliz
int isHappy(int n) {
    int current = n;
    
    // O loop continua até que o número se torne 1 (feliz) ou 4 (infeliz/ciclo).
    while (current != 1 && current != 4) {
        current = sumOfSquareDigits(current);
    }
    
    // Se o resultado final for 1, é feliz. Se for 4, é infeliz.
    return current == 1;
}

// criptografia de César
void encryptCaesar(char *message, int k) {
    int i = 0;
    while (message[i] != '\0') {
        char current_char = message[i];
        
        // Verifica se é uma letra maiúscula (A-Z)
        if (current_char >= 'A' && current_char <= 'Z') {
            // Converte o caractere para o índice 0-25
            int char_index = current_char - 'A';
            
            // Aplica o deslocamento e o módulo (26) para circularidade
            int shifted_index = (char_index + k) % 26;
            
            // Converte de volta para o caractere ASCII
            message[i] = shifted_index + 'A';
        }
        // Outros caracteres (como espaços) são mantidos inalterados
        i++;
    }
}
