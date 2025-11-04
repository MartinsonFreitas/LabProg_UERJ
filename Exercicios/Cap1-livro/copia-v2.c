#include <stdio.h>

/* copia entrada para saída; 2a. versão */
int main(void){
	int c;
	
	while ((c = getchar()) != EOF) {
		putchar (c);
	}
	
	return 0;
}
