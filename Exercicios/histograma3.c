#include <stdio.h>

#define DENTRO 1    // dentro de uma palavra
#define FORA 0      // fora de uma palavra
#define MAXIMO 50      // tamanho máximo de uma palavra

void conta_tam_palavra(int histograma[], int tam);
void imprime_histograma(int histograma[], int tam);

int main(void) {
    int histograma[MAXIMO];

    // Inicializa o histograma com zeros
    for (int i = 0; i < MAXIMO; i++) {
        histograma[i] = 0;
    }

    conta_tam_palavra(histograma, MAXIMO);
    imprime_histograma(histograma, MAXIMO);

    return 0;
}

// Preenche o histograma com a contagem do tamanho das palavras
void conta_tam_palavra(int histograma[], int tam) {
    int c, estado, tam_palavra;

    estado = FORA;
    tam_palavra = 0;

    while ((c = getchar()) != EOF) {
        if (c == ' ' || c == '\n' || c == '\t') {
            if (estado == DENTRO) {
                if (tam_palavra > 0 && tam_palavra < tam) {
                    histograma[tam_palavra]++;
                }
                tam_palavra = 0;
                estado = FORA;
            }
        } else {
            if (estado == FORA) {
                estado = DENTRO;
                tam_palavra = 1;
            } else {
                tam_palavra++;
            }
        }
    }

    // Salva a última palavra, caso a entrada termine sem separador
    if (estado == DENTRO && tam_palavra > 0 && tam_palavra < tam) {
        histograma[tam_palavra]++;
    }
}

// Imprime o histograma em formato visual
void imprime_histograma(int histograma[], int tam) {
	
    printf("Nº Caract\tNº Palavras\n");
    
    for (int i = 1; i < tam; i++) {
        if (histograma[i] > 0) {
            printf("%5d\t\t", i);
            for (int j = 0; j < histograma[i]; j++) {
                printf("*");
            }
            printf("\n");
        }
    }
    printf("\n");
}
