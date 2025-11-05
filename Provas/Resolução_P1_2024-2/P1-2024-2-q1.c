#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 100
#define MAX_BUFFER_LEN (MAX_LINE_LEN + 2) // Para \n e \0
#define MAX_WORDS_PER_LINE 5

// Protótipos das funções
int lelinha(char s[], int lim);
void inverte_string(char s[]);
int contem_numero(const char s[]);
void processa_linha(char linha[]);


/*
 * main: loop principal de leitura de linhas.
 */
int main(void) {
    char linha[MAX_BUFFER_LEN];
    int tam;

    printf("Digite as linhas de entrada (Ctrl+D para terminar):\n");

    // Loop de leitura de linhas
    while ((tam = lelinha(linha, MAX_BUFFER_LEN)) > 0) {
        // Verifica se a linha lida é maior que o limite (excluindo '\n' e '\0')
        if (tam > MAX_LINE_LEN) {
            // Mensagem de erro (c)
            fprintf(stderr, "ERRO: Linha de entrada excede %d caracteres. Processando apenas os primeiros %d.\n", MAX_LINE_LEN, MAX_LINE_LEN);
            // lelinha já limitou a leitura a MAX_LINE_LEN + 1 (para o '\n')
            // O restante da linha (após o limite) deve ser ignorado.
            // A função lelinha() do repositório não lida com o excesso, 
            // mas o problema pede para ignorar o resto.
            // Como a lelinha do repositório para no limite, o restante será lido na próxima chamada,
            // o que não é o comportamento desejado.
            // Para ignorar o resto, é necessário consumir os caracteres excedentes.
            
            // Se tam == MAX_BUFFER_LEN - 1 e o último caractere não for '\n', significa que a linha foi truncada.
            // Consome o restante da linha até encontrar um '\n' ou EOF
            if (linha[tam - 1] != '\n' && tam == MAX_BUFFER_LEN - 1) {
                int c;
                while ((c = getchar()) != EOF && c != '\n') {
                    // Ignora
                }
                // Se encontrou '\n', ajusta o tamanho para MAX_LINE_LEN
                if (c == '\n') {
                    linha[tam - 1] = '\n';
                }
            }
        }
        
        // Processa a linha lida (até o limite de MAX_LINE_LEN)
        processa_linha(linha);
    }

    return 0;
}


/* lelinha: lê uma linha da entrada, retorna o tamanho. */
int lelinha(char s[], int lim) {
    int c, i;

    for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i) {
        s[i] = c;
    }

    if (c == '\n') {
        s[i] = c;
        ++i;
    }

    s[i] = '\0';
    return i;
}

/* inverte_string: inverte a string s. */
void inverte_string(char s[]) {
    int i, j;
    char temp;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        temp = s[i];
        s[i] = s[j];
        s[j] = temp;
    }
}

/* verifica se a string contém algum dígito. Retorna 1 se contém, 0 caso contrário. */
int contem_numero(const char s[]) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (isdigit((unsigned char)s[i])) {
            return 1;
        }
    }
    return 0;
}

/* extrai palavras, inverte se necessário e imprime com formatação de 5 palavras por linha. */
void processa_linha(char linha[]) {
    char *token;
    int contador_palavras = 0;
    int palavras_na_linha = 0;

    // A função strtok modifica a string original, substituindo delimitadores por '\0'
    // Delimitadores: espaço (' '), tabulação ('\t'), nova linha ('\n')
    token = strtok(linha, " \t\n");

    while (token != NULL) {
        contador_palavras++;
        
        // Verifica se a palavra contém números
        if (!contem_numero(token)) {
            inverte_string(token);
        }

        // Imprime a palavra
        printf("%s", token);
        
        palavras_na_linha++;

        // Obtém o próximo token (palavra)
        token = strtok(NULL, " \t\n");

        // Verifica se é necessário adicionar espaço ou nova linha
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
