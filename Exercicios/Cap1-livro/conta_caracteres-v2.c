#include <stdio.h>

/* conta caracteres da entrada; 2a. versão */
int main(){
	double nc = 0;
	
	for (; getchar() != EOF; ++nc) 
		;			
	printf ("%.0f\n", nc);
	
	return 0;
}
