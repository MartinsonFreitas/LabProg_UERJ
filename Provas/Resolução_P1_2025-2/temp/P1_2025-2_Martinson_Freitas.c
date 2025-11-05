#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Definição da constante para o tamanho máximo da mensagem
#define MAX_MESSAGE_LENGTH 101 // 100 caracteres + \0

// ===================================================================
// QUESTÃO 1: VERIFICAÇÃO DE NÚMERO PRIMO
// (2.0 pontos)
// ===================================================================

/**
 * @brief Verifica se um número inteiro é primo.
 * * @param n O número a ser verificado.
 * @return true se o número for primo, false caso contrário.
 */
bool isPrime(int n) {
    if (n <= 1) {
        return false; // Números menores ou iguais a 1 não são primos.
    }
    if (n <= 3) {
        return true; // 2 e 3 são primos.
    }
    // Otimização: Ignora múltiplos de 2 e 3, verificando a partir de 5.
    if (n % 2 == 0 || n % 3 == 0) {
        return false;
    }
    
    // Verifica divisores da forma 6k ± 1 até a raiz quadrada de n.
    for (int i = 5; (long long)i * i <= n; i = i + 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

// ===================================================================
// QUESTÃO 2: CRIPTOGRAFIA DOS PRIMOS FELIZES
// (8.0 pontos)
// ===================================================================

/**
 * @brief Calcula a soma dos quadrados dos dígitos de um número.
 * * Esta é uma função auxiliar para a verificação de número feliz.
 * * @param n O número de entrada.
 * @return A soma dos quadrados de seus dígitos.
 */
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

/**
 * @brief Verifica se um número inteiro é feliz.
 * * A lógica utiliza a dica de que todo número infeliz entra em um ciclo
 * que inclui o número 4, evitando loops infinitos.
 * * @param n O número a ser verificado.
 * @return true se o número for feliz, false caso contrário.
 */
bool isHappy(int n) {
    int current = n;
    
    // O loop continua até que o número se torne 1 (feliz) ou 4 (infeliz/ciclo).
    while (current != 1 && current != 4) {
        current = sumOfSquareDigits(current);
    }
    
    // Se o resultado final for 1, é feliz. Se for 4, é infeliz.
    return current == 1;
}

/**
 * @brief Aplica a Cifra de César (Caesar Cipher) a uma mensagem.
 * * Desloca apenas letras maiúsculas (A-Z) pelo valor de K,
 * garantindo a circularidade do alfabeto.
 * * @param message A string (mensagem) a ser criptografada (modificada in-place).
 * @param k O valor de deslocamento (chave).
 */
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

/**
 * @brief Função principal da prova.
 */
int main() {
    // ===================================
    // Implementação da Questão 1
    // ===================================
    int q1_num;
    printf("--- Questão 1: Verificação de Número Primo ---\n");
    printf("Digite um número para verificar se é primo: ");
    if (scanf("%d", &q1_num) != 1) {
        fprintf(stderr, "Erro na leitura do número para a Q1.\n");
        return 1;
    }
    
    if (isPrime(q1_num)) {
        printf("%d é um número primo.\n", q1_num);
    } else {
        printf("%d não é um número primo.\n", q1_num);
    }
    printf("----------------------------------------------\n\n");
    
    // Limpa o buffer de entrada após o scanf
    while (getchar() != '\n');
    
    // ===================================
    // Implementação da Questão 2
    // ===================================
    
    // 2.a. Leitura do Número-Chave N
    int N;
    char buffer[MAX_MESSAGE_LENGTH];
    
    printf("--- Questão 2: Criptografia dos Primos Felizes ---\n");
    printf("a. Digite o Número-Chave N [1, 10000]: ");
    if (scanf("%d", &N) != 1) {
        fprintf(stderr, "Erro na leitura do Número-Chave.\n");
        return 1;
    }

    // Limpa o buffer de entrada após o scanf para a leitura da string
    while (getchar() != '\n');

    // 2.b. Verificação de Primo Feliz
    // Nota: Os requisitos de limite [1, 10000] não são verificados aqui
    // mas sim na lógica principal, seguindo as diretrizes do exame.
    bool is_prime = isPrime(N);
    bool is_happy = isHappy(N);
    bool is_happy_prime = is_prime && is_happy;

    if (!is_happy_prime) {
        printf("Não é primo feliz. Fim do programa.\n");
        return 0; // Encerra o programa se não for Primo Feliz
    }
    
    // 2.c. Criação da Chave de Criptografia K
    int K = N % 26;
    
    // Leitura da mensagem de texto
    printf("b. N é primo feliz! Chave K = %d\n", K);
    printf("c. Digite a mensagem (apenas maiúsculas, máx. 100 caracteres): ");
    
    // Leitura segura da string
    if (fgets(buffer, MAX_MESSAGE_LENGTH, stdin) == NULL) {
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
