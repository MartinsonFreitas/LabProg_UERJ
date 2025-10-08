/*
	FAETERJ-Rio
	FPR - Noite - 2022/1
	Professor Leonardo Vianna
	
	Data: 28/03/2022
	
	Quest�o 04: Implementar uma fun��o que remova todas as ocorr�ncias 
	de determinado caracter em uma string. 
*/

//importa��o de bibliotecas
#include <stdio.h>
#include <string.h>

//prot�tipos das fun��es
void removerCaracteres (char s[], char caracter);

//main
void main ()
{
	//declara��o de vari�veis
	char palavra[], sub[];
	
	//inicializando a palavra
	strcpy (palavra, "Celacanto provoca maremoto");
	
	//fazendo algumas chamadas � fun��o 'substring'
	removerCaracteres (palavra, 'O');
	printf ("palavra apos a remocao: %s\n\n", palavra);

	//inicializando a palavra
	strcpy (palavra, "ASSADOS");
	
	//fazendo algumas chamadas � fun��o 'substring'
	removerCaracteres (palavra, 'S');
	printf ("palavra apos a remocao: %s\n\n", palavra);
}

//implementa��o das fun��es
void removerCaracteres (char s[], char caracter)
{
	//declara��o de vari�veis
	int i, j;
	
	//percorrendo a string
	for (i=0;s[i]!='\0';i++)
	{
		//verificando se o caracter desejado foi encontrado
		if (s[i] == caracter)
		{
			//deslocar todos os caracteres al�m de s[i] uma posi��o � esquerda
			for (j=i;s[j]!='\0';j++)
			{
				s[j] = s[j+1];
			}
			
			i--; //descontando o incremento que ser� feito ao final da repeti��o (para que permane�a na mesma posi��o)
		}
		
	}
}
