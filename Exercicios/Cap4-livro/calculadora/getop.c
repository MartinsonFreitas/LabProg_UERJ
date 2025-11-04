#include <stdio.h>
#include <ctype.h>
#include "calc.h"

#define MAXOP 100 // tamanho máximo operando ou operador
#define NUMERO '0' // sinaliza que número foi encontrado

/* getop: obtém próximo operador ou operando numérico */
int getop(char s[]){
	
	int i, c;

	while ((s[0] = c = getch()) == ' ' || c == '\t')
		;
		
	s[1] = '\0';
	
	if (!isdigit(c) && c != '.') {
		return c; 	// não é um número
	}
	
	i = 0;
	
	if (isdigit(c)) // agrupa parte inteira
	
		while (isdigit(s[++i] = c = getch ()))
			;
			
	if (c == '.') // agrupa parte fracionária
		while (isdigit(s[++i] = c = getch()))
			;
			
	s[i] = '\0';
	
	if (c != EOF) {
		ungetch(c);
	}
	
	return NUMERO;
}
