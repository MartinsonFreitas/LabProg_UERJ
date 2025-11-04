#include<stdio.h>
#include<ctype.h>

/* atoi: converte s para inteiro; versão 2 */
int atoi (char s[])
{
	int i, n, sinal;
	
	for (i = 0; isspace(s[i]); i++) // salta espaço
		;
	
		sinal = (s [i] == '-') ? -1 : 1;
		
		if (s [i] == '+' || s[i] == '-') { // salta sinal
			i++;
		}
		
	
	for (n = 0; isdigit(s[i]); i++) {
		n = 10 * n + (s[i] - '0');
	}

	return sinal * n;
}


int main(void){
	
	printf("%d\n", atoi("     -1234"));
	return 0;
}
