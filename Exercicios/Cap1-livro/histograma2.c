#include <stdio.h>

#define DENTRO 1 // dentro de uma palavra
#define FORA 0   // fora de uma palavra
#define MAX 50   // tamanho máximo esperado de uma palavra

int main(void) {
    int c, estado, tamanho_palavra;
    int histograma[MAX];  // vetor para armazenar contagem de palavras de cada tamanho
    
    // inicializa o histograma
    for (int i = 0; i < MAX; i++) {
        histograma[i] = 0;
    }
    
    estado = FORA;
    tamanho_palavra = 0;

    while ((c = getchar()) != EOF) {
        if (c == ' ' || c == '\n' || c == '\t') {
            if (estado == DENTRO) {
                // final de palavra: salva contagem
                if (tamanho_palavra < MAX) {
                    histograma[tamanho_palavra]++;
                }
                tamanho_palavra = 0;
                estado = FORA;
            }
        } else {
            if (estado == FORA) {
                estado = DENTRO;
                tamanho_palavra = 1; // primeiro caractere da nova palavra
            } else {
                tamanho_palavra++;
            }
        }
    }

    // caso a entrada termine sem separador (última palavra)
    if (estado == DENTRO && tamanho_palavra < MAX) {
        histograma[tamanho_palavra]++;
    }

    // imprime histograma
    printf("Nº Caract\tNº Palavras\n");
    for (int i = 1; i < MAX; i++) {
        if (histograma[i] > 0) {
            printf("%5d\t\t", i);
            for (int j = 0; j < histograma[i]; j++) {
                printf("*");
            }
            printf("\n");
        }
    }

    return 0;
}
