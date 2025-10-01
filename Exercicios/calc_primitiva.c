#include <stdio.h>
#include <ctype.h>


#define MAXLINHA 100


double atof (char s[]);
int lelinha (char s[], int lim);

/* calculadora primitiva */
int main ()
{
	double soma, atof (char []);
	char linha[MAXLINHA];
	int lelinha (char linha[], int max);
		
	soma = 0;
	while (lelinha(linha, MAXLINHA) > 0){
		
		printf ("\t%g\n", soma += atof (linha));
	
	}
	
	return 0;

}

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

/* lelinha: lê uma linha em s, retorna tamanho */
int lelinha (char s[], int lim)
{
    int c, i;

    for(i = 0; i < lim - 1 && (c = getchar()) != '\n' && c != EOF; ++i) {
        s[i] = c;
    }

    if (c == '\n') {
        s[i] = c;
        ++i;
    }

    s[i] = '\0';
    return i;
}
