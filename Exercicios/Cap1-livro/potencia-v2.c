#include <stdio.h>

int pot (int m, int n);

/* testa função pot */
int main(void){
	for (int i = 0; i < 10; ++i) {
		printf ("%d %d %d\n", i, pot (2, i), pot (-3, i));
	}
	return 0;
}

/*pot: eleva a enésima potência; n>=0; versão 2 */
int pot (int base, int n){
	int p;
	
	for (p = 1; n > 0; --n) {
			p = p * base;
		}
	
	return p;
}
