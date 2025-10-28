#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 100
#define MAX_BUFFER_LEN (MAX_LINE_LEN + 2) // Para '\n' e '\0'
#define MAX_WORDS_PER_LINE 5

// Protótipos das funções
void inverte_string(char s[]);
int contem_numero(const char s[]);
void processa_linha(char linha[]);

/*
 * limpa_stdin: Consome caracteres restantes em stdin até o '\n' ou EOF.
 * Usado para ignorar o excesso de caracteres (requisito c).
 */
void limpa_stdin(void) {
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        // Ignora
    }
}

/*
 * main: loop principal de leitura de linhas usando fgets.
 */
int main(void) {
    char linha[MAX_BUFFER_LEN];
    char *retorno_fgets;

    printf("Digite as linhas de entrada (Ctrl+D para terminar):\n");

    // Loop de leitura de linhas (frases)
    while ((retorno_fgets = fgets(linha, MAX_BUFFER_LEN, stdin)) != NULL) {
        
        // Determina o tamanho da string LIDA
        size_t tam = strlen(linha);
        
        // Variavel para controlar se a linha foi truncada (excedeu 100 caracteres)
        int linha_truncada = 0;

        // 1. TRATAMENTO DE EXCESSO DE CARACTERES (Requisito c)
        // Se o buffer estiver cheio (tam == MAX_BUFFER_LEN - 1) e não terminar em '\n',
        // significa que a linha de entrada era maior que MAX_LINE_LEN.
        if (tam == MAX_BUFFER_LEN - 1 && linha[tam - 1] != '\n') {
            
            // Emite mensagem de erro (c)
            fprintf(stderr, "ERRO: Linha de entrada excede %d caracteres. Processando apenas os primeiros %d.\n", MAX_LINE_LEN, MAX_LINE_LEN);
            
            // Consome o restante da linha até o '\n' ou EOF (c)
            limpa_stdin();
            
            // Marca a linha como truncada
            linha_truncada = 1;

            // Garante que o buffer a ser processado está com '\0' no limite
            // e remove o último caractere lido (o 101º) para processar apenas os 100.
            // Aqui, linha[MAX_LINE_LEN] já está com '\0' pela chamada ao fgets,
            // mas se quiséssemos processar APENAS os 100 primeiros, faríamos:
            // linha[MAX_LINE_LEN] = '\0'; // (O fgets já faz isso no máximo de bytes lidos)

        } 
        
        // 2. REMOVE '\n' SE PRESENTE
        // Se a linha NÃO FOI TRUNCADA, o '\n' provavelmente está no final
        if (tam > 0 && linha[tam - 1] == '\n' && !linha_truncada) {
            linha[tam - 1] = '\0';
        }
        
        // 3. PROCESSAMENTO DA LINHA
        // Processa a linha lida (até o limite de MAX_LINE_LEN)
        processa_linha(linha);
    }

    return 0;
}


/* inverte_string: inverte a string s. */
void inverte_string(char s[]) {
    int i, j;
    char temp;
    
    // Calcula o comprimento da string, excluindo o '\0'
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        temp = s[i];
        s[i] = s[j];
        s[j] = temp;
    }
}

/* verifica se a string contém algum dígito. Retorna 1 se contém, 0 caso contrário. (b)*/
int contem_numero(const char s[]) {
    for (int i = 0; s[i] != '\0'; i++) {
        // Usa isdigit e o cast para compatibilidade
        if (isdigit((unsigned char)s[i])) {
            return 1;
        }
    }
    return 0;
}

/* extrai palavras, inverte se necessário e imprime com formatação de 5 palavras por linha. (a) e (d)*/
void processa_linha(char linha[]) {
    // Usando uma cópia para preservar a original para debug, se necessário, mas strtok modifica
    // a original. Vamos usar a própria 'linha' como no código anterior.
    
    char *token;
    int palavras_na_linha = 0;

    // Delimitadores: espaço (' '), tabulação ('\t'), nova linha ('\n')
    // O '\n' é usado como delimitador para garantir que a linha termine corretamente.
    // O fgets lê o '\n' se houver espaço.
    token = strtok(linha, " \t\n");

    while (token != NULL) {
        
        // 1. Tratamento da palavra: inverte se não contiver números (a e b)
        if (!contem_numero(token)) {
            inverte_string(token);
        }

        // 2. Imprime a palavra
        printf("%s", token);
        
        palavras_na_linha++;

        // 3. Obtém o próximo token (palavra)
        token = strtok(NULL, " \t\n");

        // 4. Formatação de saída (d)
        if (token != NULL) {
            // Se ainda houver palavras, verifica a formatação de 5 palavras por linha
            if (palavras_na_linha < MAX_WORDS_PER_LINE) {
                printf(" ");
            } else {
                printf("\n");
                palavras_na_linha = 0; // Reseta o contador de palavras para a nova linha
            }
        } else {
            // Se não houver mais palavras na linha de entrada, adiciona uma nova linha
            // para separar a saída de diferentes linhas de entrada.
            if (palavras_na_linha > 0) {
                 printf("\n");
            }
        }
    }
}
