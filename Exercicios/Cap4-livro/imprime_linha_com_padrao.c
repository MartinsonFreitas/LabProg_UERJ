#include <stdio.h>
#define MAXLINHA 1000 // tamanho máximo da linha

int lelinha (char linha[], int max);
int indicecad (char fonte[], char procura[]);
char padrao[] = "ar"; /* padrão a procurar */

/* procura todas as linhas que combinam com o padrão */
int main(){
	
	char linha[MAXLINHA];
	int achadas = 0;
	
	while (lelinha (linha, MAXLINHA) > 0) {
		if (indicecad (linha, padrao) >= 0) {
			printf ("%s", linha);
			achadas++;
		}
	}
	
	return achadas;

}

/* lelinha: obtém linha em s, retorna tamanho */
int lelinha (char s[], int lim){
	int c, i;
	i = 0;
	
	while (--lim > 0 && (c = getchar ()) != EOF && c != '\n') {
		s[i++] = c;
	}
	
	if (c == '\n') {
		s[i++] = c;
	}
	
	s[i] = '\0';
	return i;
}

/* indicecad: retorna índice de t em s, -1 se não tiver */
int indicecad (char s[], char t[]){
	int i, j, k;
	
	for (i = 0; s[i] != '\0'; i++) {
		for (j = i, k = 0; t[k] != '\0' && s[j] == t[k]; j++, k++)
			;
	
		if (k > 0 && t[k] == '\0') {
			return i;
		}
	}
	
	return -1;
}
