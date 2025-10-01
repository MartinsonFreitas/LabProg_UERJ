#include <ctype.h>
/* atof: converte a string s para um double */

double atof (char s[])
{
	double val, pot;
	int i, sinal;
	
	for (i = 0; isspace (s[i]); i++) // ignora espacos iniciais
		;
		
	sinal = (s[i] == '-') ? -1:1;
	if (s[i] == '+' || s[i] == '-'){
		i++;
	}
	
	for (val = 0.0; isdigit (s[i]); i++){
		val = 10.0 * val + (s[i] - '0');
	}
	
	if (s[i] == '.'){
		i++;
	}
	
	for (pot = 1.0; isdigit (s[i]); i++) {
		val = 10.0 * val + (s[i] - '0');
		pot *= 10.0;
	}
	
	return sinal * val / pot;
}

#include <stdio.h>

int main(int argc, char **argv)
{
	char str[] = "    34.56";
	double f = atof(str);
	
	printf("%f\n", f);
	
	return 0;
}

