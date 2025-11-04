/*	FAETERJ-Rio
	FPR - Fundamentos de Programação			2022/1
	Professor Leonardo Vianna
	Data: 14/03/2022
	
	STRINGS
	
	Exemplo 2: Desenvolver uma função que, dada uma string 's', determine o número
	de caracteres em 's'
*/

//importação de bibliotecas
#include <stdio.h>
#include <string.h>

#define MAX 20

//protótipos de funções
int numChar (char s[]);

//main
int main (){
		
	//declaração de variáveis
	char palavra[MAX];	//string
	int quant;
	
	//leitura dos dados de entrada
	printf ("Entre com a palavra: ");
	fflush (stdin);
	//scanf ("%s", palavra);
	fgets (palavra, MAX, stdin);
	
	//chamando a função
	quant = numChar(palavra) - 1;
	
	//exibindo o resultado 
	printf ("\n\tA string %s possui %d caracteres.\n\n", palavra, quant);
}

//implementação das funções
int numChar (char s[])
{
	//declaração de variáveis
	int i, qtChar = 0;
	
	//percorrendo toda a string
    for (i=0;s[i]!='\0';i++)
    {
        qtChar++;
    }

	//retornando a quantidade de caracteres da string
    return qtChar;
}
