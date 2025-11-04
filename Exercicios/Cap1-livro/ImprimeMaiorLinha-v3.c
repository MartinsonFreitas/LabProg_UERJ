#include <stdio.h>

#define MAXLINHA 1000 // tamanho máximo da linha

int max;				// tamanho visto até agora
char linha [MAXLINHA]; 	// linha de entrada
char maior [MAXLINHA]; 	// linha mais longa

// Protótipos das Funções
int lelinha (void);
void copia (void) ;

/* imprime maior linha entrada; versão especializada */
int main(void){
	
	int tam;
	extern int max;
	extern char maior [];
	max = 0;
	
	while ((tam = lelinha ()) > 0) {
		if (tam > max) {
			max = tam;
			copia ();
		}
	}
	
	if (max > 0) { // entrada tinha uma linha
		printf ("%s", maior);
	}
	
	return 0;
}

/* lelinha: versão especializada */
int lelinha (void){
	int c, i = 0;	
	extern char linha [];
	
	for (; i < MAXLINHA - 1 && (c = getchar())!='\n' && c!=EOF; ++i) {
		linha [i] = c;
	}
	
	if (c == '\n') {
		linha [i] = c;
		++i;
	}
	
	linha [i] = '\0';
	return i;
}

/* copia: versão especializada */
void copia (void){
	int i = 0;
	extern char linha [], maior[];
	
	
	while ((maior [i] = linha [i]) != '\0'){
		++i;
	}
}
