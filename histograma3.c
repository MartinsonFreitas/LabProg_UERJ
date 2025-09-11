#include <stdio.h>

#define DENTRO 1    // dentro de uma palavra
#define FORA 0      // fora de uma palavra
#define MAX 50      // tamanho máximo de uma palavra

// Preenche o histograma com a contagem do tamanho das palavras
void contar_tamanho_palavras(int histograma[], int max_tamanho) {
    int c, estado, tamanho_palavra;

    estado = FORA;
    tamanho_palavra = 0;

    while ((c = getchar()) != EOF) {
        if (c == ' ' || c == '\n' || c == '\t') {
            if (estado == DENTRO) {
                if (tamanho_palavra > 0 && tamanho_palavra < max_tamanho) {
                    histograma[tamanho_palavra]++;
                }
                tamanho_palavra = 0;
                estado = FORA;
            }
        } else {
            if (estado == FORA) {
                estado = DENTRO;
                tamanho_palavra = 1;
            } else {
                tamanho_palavra++;
            }
        }
    }

    // Salva a última palavra, caso a entrada termine sem separador
    if (estado == DENTRO && tamanho_palavra > 0 && tamanho_palavra < max_tamanho) {
        histograma[tamanho_palavra]++;
    }
}

// Imprime o histograma em formato visual
void imprimir_histograma(int histograma[], int max_tamanho) {
    printf("Nº Caract\tNº Palavras\n");
    for (int i = 1; i < max_tamanho; i++) {
        if (histograma[i] > 0) {
            printf("%5d\t\t", i);
            for (int j = 0; j < histograma[i]; j++) {
                printf("*");
            }
            printf("\n");
        }
    }
}

int main(void) {
    int histograma[MAX];

    // Inicializa o histograma com zeros
    for (int i = 0; i < MAX; i++) {
        histograma[i] = 0;
    }

    contar_tamanho_palavras(histograma, MAX);
    imprimir_histograma(histograma, MAX);

    return 0;
}
