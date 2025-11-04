#include <stdio.h>
#define DENTRO 1 // dentro de uma palavra
#define FORA 0 // fora de uma palavra

/* conta linhas, palavras e caracteres na entrada */
int main(void)
{
	int c, nl, np, nc;
	nl = np = nc = 0;
	int estado = FORA;
	
	while ((c = getchar()) != EOF) {
		++nc;
		
		if (c == '\n') {
			++nl;
		}
		
		if(c == ' ' || c == '\n' || c == '\t') {
			estado = FORA;
		} else if (estado == FORA) {
			estado = DENTRO;
			++np;
		}
	}
	
	printf ("%d %d %d\n", nl, np, nc);
	return 0;
}
