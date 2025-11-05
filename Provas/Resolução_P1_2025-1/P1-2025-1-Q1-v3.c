#include <stdio.h>
// Não precisamos do <stdlib.h> pois usaremos a nossa própria função atoi

// 0. Funcao atoi customizada (converte string de dígitos para inteiro)
/* atoi: converte s em um inteiro */
int atoi (char s [])
{
    int n = 0;
    // O loop continua enquanto o caractere atual for um dígito ('0' a '9')
    for (int i = 0; s[i] >= '0' && s[i] <= '9'; ++i) {
        // Acumulação: multiplica o valor atual por 10 e adiciona o novo dígito.
        // O valor numérico do dígito é obtido subtraindo '0' do caractere.
        n = 10 * n + (s[i] - '0');
    }
    return n;
}

// 1. Função para verificar se um número é primo (inalterada)
// Retorna 1 (verdadeiro) se for primo, 0 (falso) caso contrário.
int eh_primo(int num) {
    if (num <= 1) {
        return 0;
    }
    if (num == 2) {
        return 1;
    }
    if (num % 2 == 0) {
        return 0;
    }

    // Otimização: verifica divisibilidade apenas por ímpares até a raiz quadrada.
    for (int i = 3; i * i <= num; i = i + 2) {
        if (num % i == 0) {
            return 0; // Não é primo
        }
    }
    return 1; // É primo
}

// 2. Função que gerencia a I/O com getchar() e o processamento de primos
long long processar_entrada_e_somar() {
    int c;
    char buffer[20] = {0}; // Buffer para armazenar dígitos (string)
    int buffer_index = 0;
    int num_lido[2] = {0}; // Array para armazenar num1 e num2
    int num_count = 0;
    int i; // Variável para loops

    printf("Digite dois numeros separados por espaco/nova linha, seguido de EOF (Ctrl+D ou Ctrl+Z):\n");

    // Loop principal: Lendo a entrada caractere por caractere com getchar()
    while ((c = getchar()) != EOF) {
        putchar(c); // Echo de volta para o usuário

        // A. Se o caractere é um dígito
        if (c >= '0' && c <= '9') {
            if (buffer_index < 19) {
                buffer[buffer_index++] = c;
            }
        }
        // B. Se o caractere é um separador e temos dígitos para processar
        else if ((c == ' ' || c == '\n' || c == '\t') && buffer_index > 0) {
            if (num_count < 2) {
                // 1. Finaliza a string
                buffer[buffer_index] = '\0';
                
                // 2. CONVERSÃO USANDO NOSSA FUNÇÃO ATOI ANEXA
                num_lido[num_count] = atoi(buffer);
                num_count++;

                // 3. Limpa o buffer para o próximo número
                buffer_index = 0;
                for (i = 0; i < 20; i++) buffer[i] = 0;

                // 4. Se já leu os dois números, podemos sair do loop de leitura
                if (num_count == 2) {
                    break;
                }
            }
        }
        // C. Ignora outros caracteres (a menos que sejam dígitos ou separadores)
    }
    
    // CASO ESPECIAL: Se o usuário digitou o segundo número e pressionou EOF imediatamente
    if (num_count == 1 && buffer_index > 0) {
        buffer[buffer_index] = '\0';
        num_lido[num_count] = atoi(buffer);
        num_count++;
    }
    
    printf("\n"); // Nova linha após a leitura

    // 3. Lógica de Processamento e Cálculo da Soma
    if (num_count < 2) {
        printf("Erro: Nao foram fornecidos dois numeros inteiros validos.\n");
        return 0;
    }
    
    // Ordenação e ajuste dos limites
    int inicio, fim;
    if (num_lido[0] <= num_lido[1]) {
        inicio = num_lido[0];
        fim = num_lido[1];
    } else {
        inicio = num_lido[1];
        fim = num_lido[0];
    }
    if (inicio < 2) {
        inicio = 2;
    }

    // Cálculo da soma
    long long soma_primos = 0;

    printf("--- Calculo da Soma dos Primos ---\n");
    printf("Intervalo de busca (inclusive): %d a %d\n", inicio, fim);
    printf("Numeros primos encontrados: ");

    for (i = inicio; i <= fim; i++) {
        if (eh_primo(i)) {
            soma_primos += i;
            printf("%d ", i);
        }
    }
    printf("\n");

    return soma_primos;
}

// 4. Função principal (main) - Coordenadora
int main() {
    long long resultado_soma;

    // A main chama a função que gerencia toda a leitura e cálculo
    resultado_soma = processar_entrada_e_somar();

    // A main exibe o resultado
    if (resultado_soma > 0) {
        printf("\nA soma final dos primos no intervalo e: %lld\n", resultado_soma);
    } 

    return 0;
}
