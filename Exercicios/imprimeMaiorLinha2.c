#include <stdio.h>
#include <string.h>

#define MAXLINHA 10 //tamanho máximo da linha entrada

int lelinha (char linha[], int maxlinha);
void copia (char para[], char de[]);

/* imprime maior linha entrada */
int main(void)
{

	int tam = 0; // tamanho atual da linha
	int max; // tamanho máximo visto até agora
	char linha [MAXLINHA]; // linha atual
	char maior [MAXLINHA]; // maior linha guardada
	
	while ((tam = lelinha (linha, MAXLINHA)) > 0) {
		if (tam >= max) { // Se a linha for igual a anterior, SUBSTITUI a anterior
			max = tam;
			copia (maior, linha);
		}
	}
	
	if (max > 0) { // entrada tinha uma linha
		printf ("%s", maior);
	}	
	
	return 0;
}

/* lelinha: lê uma linha em s, retorna tamanho */
int lelinha (char s[], int lim)
{
	//int c, i = 0, j = 0; // zerando as variáveis para não dar erro de segmentação
	int c, i, j;
	
	for (i = 0, j = 0; (c = getchar()) != EOF && c != '\n'; j++){
    //while ((c = getchar()) != EOF && c != '\n') { // Faz o laço para a leitura dos caracteres
        if (i < lim-1) {   // copia só até (lim - 1)
            s[i++] = c;
        }
        //j++; // conta todos os caracteres, mesmo os que não couberam
    }

    if (c == '\n') {
        if (i < lim-1) {   // só adiciona '\n' se couber
            s[i++] = c;
        }
        j++; // também conta o '\n'
    }

    s[i] = '\0'; // finaliza a string
    return j;    // retorna o tamanho total da linha lida
	
}

/* copia: copia 'de' -> 'para';
presume que 'para' é grande o suficiente */
void copia (char para[], char de[])
{
	int i = 0;
	
	while ((para[i] = de[i]) != '\0') {
	++i;
	}

}
