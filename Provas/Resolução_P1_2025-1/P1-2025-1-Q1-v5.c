#include <stdio.h>

// 1. Função para verificar se um número é primo (inalterada)
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

// 2. NOVA FUNÇÃO: Ler um único número (inteiro positivo) usando getchar()
// e sem usar array de buffer (utiliza ungetc para simplificar a separação).
// Retorna o número lido.
int ler_numero() {
    int c;
    int n = 0; // O número inteiro a ser construído

    // 2.1. Ignora espaços, tabs ou novas linhas (separadores) antes do número
    while ((c = getchar()) != EOF && (c == ' ' || c == '\n' || c == '\t')) {
        // Echo do separador lido
        putchar(c);
    }
    
    // 2.2. Se chegou ao EOF antes de ler um dígito, retorna 0 ou um valor de erro
    if (c == EOF) {
        return -1; // Usamos -1 para sinalizar que a leitura falhou/acabou
    }

    // 2.3. Se o primeiro caractere encontrado for um dígito, começa a conversão
    if (c >= '0' && c <= '9') {
        // Converte e acumula o primeiro dígito
        n = c - '0';
        putchar(c); // Echo

        // 2.4. Continua lendo e acumulando os dígitos subsequentes
        while ((c = getchar()) != EOF && c >= '0' && c <= '9') {
            n = 10 * n + (c - '0');
            putchar(c); // Echo
        }
    } else {
        // Se o caractere não é dígito nem separador, ignora e retorna falha.
        // Já demos echo do caractere inicial (c), mas como não é dígito, é erro.
        return -1;
    }

    // 2.5. Quando o loop termina (c não é mais um dígito ou é EOF):
    //      - Se c não é EOF, devolvemos o caractere separador ou inválido
    //        ao stream de entrada para que a próxima chamada de ler_numero()
    //        o processe (ou o descarte como separador).
    if (c != EOF) {
        ungetc(c, stdin);
    }

    // Retorna o número construído
    return n;
}

// 3. NOVA FUNÇÃO: Coordena a leitura e o cálculo (substitui processar_entrada_e_somar)
long long ler_limites_e_somar() {
    // 3.1. Leitura dos dois números usando a nova função ler_numero()
    printf("Digite dois numeros separados por espaco/nova linha, seguido de EOF (Ctrl+D ou Ctrl+Z):\n");
    
    int num1 = ler_numero();
    int num2 = ler_numero();
    
    // Simula a leitura do EOF para finalizar a saída
    while(getchar() != EOF); 
    printf("\n"); // Nova linha após a leitura da entrada

    // 3.2. Validação da Leitura
    if (num1 == -1 || num2 == -1) {
        printf("Erro: Nao foram fornecidos dois numeros inteiros validos ou EOF precoce.\n");
        return 0; 
    }
    
    // 3.3. Ordenação e ajuste dos limites
    int inicio, fim;
    if (num1 <= num2) {
        inicio = num1;
        fim = num2;
    } else {
        inicio = num2;
        fim = num1;
    }
    if (inicio < 2) {
        inicio = 2;
    }

    // 3.4. Cálculo da soma
    long long soma_primos = 0;
    int i; 

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

// 4. Função principal (main) - Coordenadora final
int main() {
    long long resultado_soma;

    // A main chama a função que gerencia toda a leitura e cálculo
    resultado_soma = ler_limites_e_somar();

    // A main exibe o resultado
    if (resultado_soma > 0) {
        printf("\nA soma final dos primos no intervalo e: %lld\n", resultado_soma);
    } 

    return 0;
}
