/*
 	a) Função que faz a conversão correta para letras maiúsculas de todas as letras (1pt)
 	
		1: ler as letras
		2: converter para maiúscula
			// islower(c) letra minúscula
			// isupper(c) letra maiúscula
		
		b) Função que elimina todos os caracteres diferentes de letra, vírgula, ponto e espaço, 		
		deixando apenas a primeira ocorrência de cada um desses caracteres (2pts)
		
		c) Função que completa a chave corretamente com os caracteres que faltam. (3pts)

 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

//definição de constantes
#define TRUE 1
#define FALSE 0

// toupper -> coloca em maiusculo
// compara os caracteres da chave com o alfabeto
// retira a duplicidade do alfabeto
// concatena a chave com o restante do alfabeto

int main(void) {
	
	//protótipos das funções
void removerCaracteres (char s[], char caracter);

//main
void main ()
{
	//declaração de variáveis
	char chave[30];
	
	//inicializando a palavra
	strcpy (palavra, "Celacanto provoca maremoto");
	
	//fazendo algumas chamadas à função 'substring'
	removerCaracteres (palavra, 'O');
	printf ("palavra apos a remocao: %s\n\n", palavra);

	//inicializando a palavra
	strcpy (palavra, "ASSADOS");
	
	//fazendo algumas chamadas à função 'substring'
	removerCaracteres (palavra, 'S');
	printf ("palavra apos a remocao: %s\n\n", palavra);
	
    return 0;
}

//implementação das funções
void removerCaracteres (char s[], char caracter) {
	
	int caracter;

		while ((caracter = getchar()) != EOF) {
			if (caracter == 'A-Z' || caracter == 'a-z' || caracter == ' ' || caracter == '.' || caracter == ',') {
				
			}
		}

	
	/*
	 * //declaração de variáveis
	int i, j;
	
	//percorrendo a string
	for (i=0;s[i]!='\0';i++)
	{
		//verificando se o caracter desejado foi encontrado
		if (s[i] == caracter)
		{
			//deslocar todos os caracteres além de s[i] uma posição à esquerda
			for (j=i;s[j]!='\0';j++)
			{
				s[j] = s[j+1];
			}
			
			i--; //descontando o incremento que será feito ao final da repetição (para que permaneça na mesma posição)
		}
		
	}
	*/
	
}

void comprime(char s[], char t[]) {
    int i, j, k;

    //percorrendo as strings
    for (i = j = 0; s[i] != '\0'; i++) {
		
        // Assume que o caractere s[i] não está em t
        int esta_presente = FALSE;

        // Verifica se s[i] está presente em t
        for (k = 0; t[k] != '\0'; k++) {
            if (s[i] == t[k]) {
                esta_presente = TRUE;
                break; // Caractere encontrado => sai do 2º "for" => volta pro 1º
            }
        }

        // Se não estiver em s, copia para a posição 'j'
        if (!esta_presente) {
            s[j] = s[i];
            j++;
        }
    }

    // Adiciona '\0' -> fim da nova string
    s[j] = '\0';
}
