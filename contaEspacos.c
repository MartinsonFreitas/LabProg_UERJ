#include <stdio.h>

int main(int argc, char **argv)
{
	int ndigito [10];
	
	for (int i = 0; i < 10; ++i) {
		ndigito [i] = 0;
	}
		
	int c, nbranco, noutro;
	nbranco = noutro = 0;
	
	while ((c = getchar()) != EOF)
		if (c >= '0' && c <= '9') {
			++ndigito[c - '0'];
		} else if (c == ' ' || c == '\n' || c == '\t') {
			++nbranco;
		} else {
			++noutro;
		}

	printf ("dígitos =");
	
	for (int i = 0; i < 10; ++i) {
		printf (" %d", ndigito[i]);
	}
	
	printf (", espaço branco=%d, outros=%d\n", nbranco, noutro);
	return 0;
}

