#include<stdio.h>
#include<ctype.h>

/* lower: converte c para minúsculo; só para ASCII */
int lower (int c){
	
	if (c >= 'A' && c <= 'Z') {
		return c + 'a' - 'A';
	}

	return c;
}

int main(void){
	
	palavra = lower("C");
	
	printf(palavra);
	
	return 0;
}



unsigned long int prox = 1;

/*rand:retorna inteiro pseudo-randômico entre 0..32767 */
int rand (void){
	prox = prox * 1103515245 + 12345;
	return (unsigned int) (prox / 65536) % 32768;
} 

/* srand: define semente para rand () */
void srand (unsigned int semente) {
	prox = semente;
}
