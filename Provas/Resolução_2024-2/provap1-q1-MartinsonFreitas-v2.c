#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 100
#define MAX_BUFFER (MAX_LINE + 2) // Para \n e \0
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
    char linha[MAX_BUFFER];
    int tam;

    printf("Digite as linhas de entrada (Ctrl+D para terminar):\n");

    // Loop de leitura de linhas
    while ((tam = lelinha(linha, MAX_BUFFER)) > 0) {
        // Verifica se a linha lida é maior que o limite (excluindo '\n' e '\0')
        if (tam > MAX_LINE) {
            // Mensagem de erro (c)
            printf("ERRO: Linha de entrada excede %d caracteres. Processando apenas os primeiros %d.\n", MAX_LINE, MAX_LINE);
            // lelinha já limitou a leitura a MAX_LINE + 1 (para o '\n')
            // O restante da linha (após o limite) deve ser ignorado.
            // A função lelinha() do repositório não lida com o excesso, 
            // mas o problema pede para ignorar o resto.
            // Como a lelinha do repositório para no limite, o restante será lido na próxima chamada,
            // o que não é o comportamento desejado.
            // Para ignorar o resto, é necessário consumir os caracteres excedentes.
            
            // Se tam == MAX_BUFFER - 1 e o último caractere não for '\n', significa que a linha foi truncada.
            // Consome o restante da linha até encontrar um '\n' ou EOF
            if (linha[tam - 1] != '\n' && tam == MAX_BUFFER - 1) {
                int c;
                while ((c = getchar()) != EOF && c != '\n') {
                    // Ignora
                }
                // Se encontrou '\n', ajusta o tamanho para MAX_LINE
                if (c == '\n') {
                    linha[tam - 1] = '\n';
                }
            }
        }
        
        // Processa a linha lida (até o limite de MAX_LINE)
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

/* contem_numero: verifica se a string contém algum dígito. Retorna 1 se contém, 0 caso contrário. */
int contem_numero(const char s[]) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (isdigit((unsigned char)s[i])) {
            return 1;
        }
    }
    return 0;
}

/* processa_linha: extrai palavras, inverte se necessário e imprime com formatação de 5 palavras por linha.  */
void processa_linha(char linha[]) {
    char palavra_atual[MAX_LINE]; // Buffer para armazenar a palavra extraída
    int palavras_na_linha_saida = 0;
    int i = 0; // Índice para percorrer a string de entrada 'linha[]'

    // Loop principal: percorre toda a string de entrada
    while (linha[i] != '\0') {
        
        // Pula delimitadores (espaço, tabulação, nova linha)
        while (linha[i] == ' ' || linha[i] == '\t' || linha[i] == '\n') {
            i++;
        }
        
        // Se chegamos ao final da string após pular delimitadores, saímos.
        if (linha[i] == '\0') {
            break;
        }

        // Extrai a palavra
        int j = 0; // Índice para a palavra atual
        
        // Copia caracteres até encontrar um delimitador ou o fim da string
        while (linha[i] != '\0' && linha[i] != ' ' && linha[i] != '\t' && linha[i] != '\n') {
            palavra_atual[j] = linha[i];
            i++;
            j++;
        }
        
        // Garante que a palavra extraída seja uma string válida (termina em '\0')
        palavra_atual[j] = '\0';
        // Agora, 'palavra_atual' contém uma palavra

        // Processamento da palavra
        if (j > 0) { // Garante que algo foi extraído
            
            // Inverte se não contiver números
            if (!contem_numero(palavra_atual)) {
                inverte_string(palavra_atual);
            }

            // Imprime a palavra
            printf("%s", palavra_atual);
            palavras_na_linha_saida++;

            // Formatação de saída
            if (palavras_na_linha_saida < MAX_WORDS_PER_LINE) {
                // Se houver mais palavras a processar na entrada (linha[i] != '\0')
                // e o próximo caractere não for um delimitador, adiciona espaço.
                // Mas, para simplificar, verificamos se AINDA não chegamos ao limite de 5
                // e o próximo não é '\0' (mesmo que seja delimitador, vamos pular).
                
                // Se o próximo caractere após a palavra atual NÃO for '\0',
                // significa que haverá mais palavras ou delimitadores. Adiciona espaço.
                if (linha[i] != '\0') {
                    printf(" ");
                }
                
            } else {
                // Se atingiu 5 palavras, força a nova linha e reseta o contador
                printf("\n");
                palavras_na_linha_saida = 0; 
            }
        }
    }
    
    // Se a última linha de saída não foi terminada por '\n' - Adiciona.
    if (palavras_na_linha_saida > 0) {
        printf("\n");
    }
}
