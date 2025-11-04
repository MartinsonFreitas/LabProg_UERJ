#include <stdio.h>

/* conta caracteres da entrada; 2a. versão */
int main(){
	
	long nc = 0;
	
	while (getchar() != EOF) {
		++nc;
	}
	
	printf ("%ld\n", nc);
	return 0;
}
