#include<stdio.h>

/* atoi: converte s em um inteiro */
int atoi (char s []){
	int n = 0;
	
	for (int i = 0; s[i] >= '0' && s[i] <= '9'; ++i) {
		
		n = 10 * n + (s[i] - '0');
	}
		
	return n;
}

int main(void){
	
	printf("%d\n", atoi("9"));
	
	return 0;
}
