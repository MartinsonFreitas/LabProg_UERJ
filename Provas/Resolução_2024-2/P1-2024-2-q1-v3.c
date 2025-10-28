#include <stdio.h>
#include <ctype.h>

// Definições necessárias para o problema
#define MAX_LINE_LEN 100
#define MAX_BUFFER_LEN (MAX_LINE_LEN + 2) // Para \n e \0
#define MAX_WORDS_PER_LINE 5
#define MAX_PALAVRAS_TOTAL (MAX_LINE_LEN / 2) // Estimativa de palavras que caberiam

// Protótipos das funções
int lelinha(char s[], int lim);
void inverte_string(char s[]);
int contem_numero(const char s[]);
void processa_linha(char linha[]);
int calcula_tamanho(const char s[]); 
int eh_delimitador(char c);

/*
 * main: loop principal de leitura de linhas, adaptado para processar
 * cada linha lida imediatamente.
 */
int main(void) {
    char linha[MAX_BUFFER_LEN];
    int tam;

    printf("Digite as linhas de entrada (Ctrl+D para terminar):\n");

    // Loop de leitura de linhas
    while ((tam = lelinha(linha, MAX_BUFFER_LEN)) > 0) {
        // Bloco de tratamento de erro (Linha c. do problema)
        // Se tam == MAX_BUFFER_LEN - 1 e o último caractere não for '\n', 
        // a linha foi truncada, indicando que era maior que MAX_LINE_LEN.
        if (tam == MAX_BUFFER_LEN - 1 && linha[tam - 1] != '\n') {
            fprintf(stderr, "ERRO: Linha de entrada excede %d caracteres. Processando apenas os primeiros %d.\n", MAX_LINE_LEN, MAX_LINE_LEN);
            
            // Consome o restante da linha até encontrar um '\n' ou EOF
            int c;
            while ((c = getchar()) != EOF && c != '\n') {
                // Ignora
            }
            // Se encontrou '\n', ajusta o buffer para ter o '\n' (se necessário)
            if (c == '\n') {
                linha[tam - 1] = '\n';
            }
        }
        
        // Processa e imprime a linha lida (a lógica de processamento foi mantida)
        processa_linha(linha);
    }

    return 0;
}


/* lelinha: lê uma linha em s, retorna tamanho. (Versão do usuário adaptada) */
int lelinha (char s[], int lim) {
    int c, i;

    // A condição de parada original do usuário era: c != '\n' && c != EOF
    for(i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i) {
        s[i] = c;
    }
    
    if (c == '\n') {
        s[i] = c;
        ++i;
    }
    
    s[i] = '\0';
    return i;
}

/* calcula_tamanho: retorna o tamanho da string s, substituindo strlen. */
int calcula_tamanho(const char s[]) {
    int i = 0;
    while (s[i] != '\0') {
        i++;
    }
    return i;
}

/* inverte_string: inverte a string s usando índices. */
void inverte_string(char s[]) {
    int i, j;
    char temp;
    int len = calcula_tamanho(s); 

    for (i = 0, j = len - 1; i < j; i++, j--) {
        temp = s[i];
        s[i] = s[j];
        s[j] = temp;
    }
}

/* contem_numero: verifica se a string contém algum dígito. Retorna 1 se contém, 0 caso contrário. */
int contem_numero(const char s[]) {
    int i;
    for (i = 0; s[i] != '\0'; i++) {
        // Usa isdigit e manipulação de índice, sem ponteiros
        if (isdigit((unsigned char)s[i])) {
            return 1;
        }
    }
    return 0;
}

/* eh_delimitador: verifica se o caractere é um delimitador. */
int eh_delimitador(char c) {
    // Delimitadores: espaço (' '), tabulação ('\t'), nova linha ('\n'), fim de string ('\0')
    return (c == ' ' || c == '\t' || c == '\n' || c == '\0');
}

/* * processa_linha: extrai palavras, inverte se necessário e imprime com formatação de 5 palavras por linha.
 * Evita o uso de 'char *' e 'strtok'.
 */
void processa_linha(char linha[]) {
    char palavras[MAX_PALAVRAS_TOTAL][MAX_LINE_LEN + 1]; 
    int indice_linha = 0; 
    int palavra_atual = 0; 
    int indice_palavra = 0; 
    
    // 1. Extração de palavras (Substituindo strtok por lógica de índice)
    while (linha[indice_linha] != '\0' && palavra_atual < MAX_PALAVRAS_TOTAL) {
        // Pula delimitadores iniciais
        while (eh_delimitador(linha[indice_linha])) {
            indice_linha++;
        }

        if (linha[indice_linha] == '\0') {
            break; 
        }

        // Copia a palavra
        indice_palavra = 0;
        while (!eh_delimitador(linha[indice_linha]) && indice_palavra < MAX_LINE_LEN) {
            palavras[palavra_atual][indice_palavra] = linha[indice_linha];
            indice_palavra++;
            indice_linha++;
        }
        
        palavras[palavra_atual][indice_palavra] = '\0';
        
        if (indice_palavra > 0) {
            palavra_atual++;
        }
    }
    
    // 2. Processamento (Inversão) e Impressão
    int total_palavras = palavra_atual;
    int palavras_na_linha = 0;
    
    for (int i = 0; i < total_palavras; i++) {
        // Inverte se não contiver número
        if (!contem_numero(palavras[i])) {
            inverte_string(palavras[i]);
        }

        printf("%s", palavras[i]);
        
        palavras_na_linha++;

        // Lógica de formatação de 5 palavras por linha
        if (i < total_palavras - 1) { // Se não for a última palavra
            if (palavras_na_linha < MAX_WORDS_PER_LINE) {
                printf(" "); // Adiciona espaço
            } else {
                printf("\n"); // Adiciona nova linha (chegou em 5)
                palavras_na_linha = 0; // Reseta o contador
            }
        }
    }
    
    // 3. Finaliza a linha de saída se houver palavras e a linha não tiver sido 
    // finalizada por uma quebra de linha de 5 palavras.
    if (total_palavras > 0 && palavras_na_linha > 0) {
        printf("\n");
    }
}
