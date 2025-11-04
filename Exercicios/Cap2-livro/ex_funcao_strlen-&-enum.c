/* strlen: retorna tamanho de s */
int strlen (char s[]){
	int i;
	i = 0;
	
	while (s[i] != '\0') {
		++i;
	}
	
	return i
}


// Exemplos de enum
enum boolean {NAO, SIM};

enum escapes{BELL='\a', RETROCESSO='\b', TAB='\t',
				NOVALINHA='\n', TABV='\v', RETORNO='\r' };
	
enum meses {JAN=1, FEV, MAR, ABR, MAI, JUN,
				JUL, AGO, SET, OUT, NOV, DEZ};
			/* FEV = 2, MAR = 3, etc.*/
