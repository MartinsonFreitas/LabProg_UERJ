#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Constantes definidas conforme a prova e a análise
#define MAX_INPUT_LINE 101 // 100 caracteres + '\0'
#define MAX_OUTPUT_WIDTH 20
#define MAX_OUTPUT_BUFFER 500 // Buffer grande o suficiente para a saída convertida

/*
 * lelinha: lê uma linha em s, com limite lim.
 * Retorna o tamanho da linha lida (excluindo '\0').
 * Implementa a lógica de erro e descarte para linhas > 100.
 */
int lelinha(char s[], int lim)
{
    int c, i;
    int len = 0;
    int excedeu = 0;

    // Lê até o limite - 1, ou '\n', ou EOF
    for (i = 0; i < lim - 1 && (c = getchar()) != '\n' && c != EOF; ++i) {
        s[i] = c;
    }
    len = i;

    // Verifica se a linha excedeu o limite
    if (c != '\n' && c != EOF) {
        excedeu = 1;
        // Descarta o resto da linha
        while ((c = getchar()) != '\n' && c != EOF) {
            // Ignora
        }
    }

    // Adiciona o '\n' se foi lido dentro do limite
    if (c == '\n' && len < lim - 1) {
        s[len] = c;
        len++;
    }

    s[len] = '\0';

    if (excedeu) {
        printf("ERRO: Linha de entrada excedeu 100 caracteres. Apenas os 100 primeiros foram processados.\n");
    }

    return len;
}

/*
 * eh_separador: verifica se o caractere é um separador de palavra.
 */
int eh_separador(char c)
{
    return (c == ' ' || c == ';' || c == ',' || c == '.' || c == '\0' || c == '\n');
}

/*
 * substitui_palavras: realiza as substituições de palavras na linha de entrada.
 * Retorna o tamanho da string de saída.
 */
int substitui_palavras(const char entrada[], char saida[])
{
    int i = 0; // Índice de leitura da entrada
    int j = 0; // Índice de escrita na saída
    int len_entrada = strlen(entrada);

    while (i < len_entrada) {
        // Tenta substituir "ponto" ou "pontos" -> "vocábulo" ou "vocábulos"
        if (i + 5 <= len_entrada && strncmp(&entrada[i], "ponto", 5) == 0) {
            // Verifica se é "pontos"
            if (i + 6 <= len_entrada && entrada[i+5] == 's' && eh_separador(entrada[i+6])) {
                strcpy(&saida[j], "vocábulos");
                j += strlen("vocábulos");
                i += 6;
            }
            // Verifica se é "ponto"
            else if (eh_separador(entrada[i+5])) {
                strcpy(&saida[j], "vocábulo");
                j += strlen("vocábulo");
                i += 5;
            }
            // Se não for "ponto" nem "pontos" com separador, trata como caractere normal
            else {
                saida[j++] = entrada[i++];
            }
        }
        // Tenta substituir "reta" ou "retas" -> "frase" ou "frases"
        else if (i + 4 <= len_entrada && strncmp(&entrada[i], "reta", 4) == 0) {
            // Verifica se é "retas"
            if (i + 5 <= len_entrada && entrada[i+4] == 's' && eh_separador(entrada[i+5])) {
                strcpy(&saida[j], "frases");
                j += strlen("frases");
                i += 5;
            }
            // Verifica se é "reta"
            else if (eh_separador(entrada[i+4])) {
                strcpy(&saida[j], "frase");
                j += strlen("frase");
                i += 4;
            }
            // Se não for "reta" nem "retas" com separador, trata como caractere normal
            else {
                saida[j++] = entrada[i++];
            }
        }
        // Tenta substituir "plano" ou "planos" -> "parágrafo" ou "parágrafos"
        else if (i + 5 <= len_entrada && strncmp(&entrada[i], "plano", 5) == 0) {
            // Verifica se é "planos"
            if (i + 6 <= len_entrada && entrada[i+5] == 's' && eh_separador(entrada[i+6])) {
                strcpy(&saida[j], "parágrafos");
                j += strlen("parágrafos");
                i += 6;
            }
            // Verifica se é "plano"
            else if (eh_separador(entrada[i+5])) {
                strcpy(&saida[j], "parágrafo");
                j += strlen("parágrafo");
                i += 5;
            }
            // Se não for "plano" nem "planos" com separador, trata como caractere normal
            else {
                saida[j++] = entrada[i++];
            }
        }
        // Copia o caractere se não houver substituição
        else {
            saida[j++] = entrada[i++];
        }
    }

    saida[j] = '\0';
    return j;
}

/*
 * formata_saida: imprime a string com quebra de linha a cada MAX_OUTPUT_WIDTH (20).
 * A quebra deve ocorrer no último espaço antes do limite, ou no limite se não houver espaço.
 */
void formata_saida(const char s[])
{
    int i = 0;
    int len = strlen(s);

    while (i < len) {
        int limite_linha = i + MAX_OUTPUT_WIDTH;
        int quebra = limite_linha;

        // Se o limite exceder o fim da string, a quebra é no fim
        if (limite_linha >= len) {
            quebra = len;
        } else {
            // Procura o último espaço antes do limite_linha
            int k;
            for (k = limite_linha - 1; k > i; k--) {
                if (s[k] == ' ') {
                    quebra = k;
                    break;
                }
            }
            // Se não encontrou espaço, a quebra é no limite (corta a palavra)
            if (k == i) {
                quebra = limite_linha; // Quebra forçada
            }
        }

        // Imprime o segmento
        for (int k = i; k < quebra; k++) {
            printf("%c", s[k]);
        }
        printf("\n");

        // Avança para o próximo segmento
        i = quebra;
        // Se a quebra foi em um espaço, avança mais um para não imprimir o espaço
        if (s[i] == ' ') {
            i++;
        }
    }
}

/*
 * main: lê linhas até EOF, processa e formata a saída.
 */
int main(void)
{
    char linha_entrada[MAX_INPUT_LINE];
    char linha_saida[MAX_OUTPUT_BUFFER];
    int len;

    printf("Digite o texto (Ctrl+D para terminar):\n");

    // Loop de leitura até EOF
    while ((len = lelinha(linha_entrada, MAX_INPUT_LINE)) > 0) {
        // Remove o '\n' do final se existir, para evitar problemas na substituição
        if (len > 0 && linha_entrada[len - 1] == '\n') {
            linha_entrada[len - 1] = '\0';
        }

        // Realiza a substituição
        substitui_palavras(linha_entrada, linha_saida);

        // Formata e imprime a saída
        formata_saida(linha_saida);
    }

    return 0;
}

