#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 100
#define MAX_BUFFER_LEN (MAX_LINE_LEN + 2) // Para \n e \0

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
    int c;

    printf("Digite as linhas de entrada (Ctrl+D para terminar):\n");

    // Loop de leitura de linhas
    while ((tam = lelinha(linha, MAX_BUFFER_LEN)) > 0) {
        
        // Verifica se a linha lida foi truncada (tam = MAX_BUFFER_LEN - 1 = 101) 
        // e se o último caractere não foi um '\n'. 
        // Isso indica que a linha de entrada é maior que MAX_LINE_LEN (100).
        if (tam == MAX_BUFFER_LEN - 1 && linha[tam - 1] != '\n') {
            
            // 1. Emite mensagem de erro (c)
            fprintf(stderr, "ERRO: Linha de entrada excede %d caracteres. Processando apenas os primeiros %d.\n", MAX_LINE_LEN, MAX_LINE_LEN);
            
            // 2. Garante que a linha a ser processada termina em '\0', truncada em MAX_LINE_LEN.
            // O último caractere lido em linha[tam - 1] deve ser substituído por '\0' 
            // para que a string tenha no máximo MAX_LINE_LEN caracteres de dados.
            linha[MAX_LINE_LEN] = '\0';
            
            // 3. Ignora o restante da linha de entrada (c).
            // Consome caracteres até encontrar '\n' ou EOF
            while ((c = getchar()) != EOF && c != '\n') {
                // Ignora
            }
            // Se encontrou '\n', a próxima chamada a lelinha começa corretamente.
        }
        
        // Se a linha lida termina em '\n', garante que o '\n' é removido para que
        // 'processa_linha' consiga usar strtok corretamente em todas as palavras.
        // A função processa_linha já lida com o '\n' como delimitador, então este passo
        // não é estritamente necessário para o funcionamento, mas melhora a clareza.
        
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

/* verifica se a string contém algum dígito. Retorna 1 se contém, 0 caso contrário. (b)*/
int contem_numero(const char s[]) {
    for (int i = 0; s[i] != '\0'; i++) {
        // Usa isdigit e o cast para compatibilidade C/C++ (embora o código seja C)
        if (isdigit((unsigned char)s[i])) {
            return 1;
        }
    }
    return 0;
}

/* extrai palavras, inverte se necessário e imprime com formatação de 5 palavras por linha. (a) e (d)*/
void processa_linha(char linha[]) {
    char *token;
    int palavras_na_linha = 0;
    const int MAX_WORDS_PER_LINE = 5;

    // A função strtok modifica a string original, substituindo delimitadores por '\0'
    // Delimitadores: espaço (' '), tabulação ('\t'), nova linha ('\n')
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
