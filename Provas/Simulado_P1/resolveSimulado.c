/*
 * resolvesimulado.c 
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define MAX 100

void cria_chave(char chave[], char frase[]);
void criptografar(char frase_cripto[], char chave[], char frase[]);


int main()
{
	char frase[MAX] = {'\0'};
	char chave[MAX] = {'\0'};
	char frase_cripto[MAX] = {'\0'};

	printf("Digite uma frase para criar a chave criptográfica: ");
	fgets(frase, MAX, stdin);
	printf("A frase é: %s", frase);
	
	cria_chave(chave, frase);
	printf("A frase é: %s\n", chave);
	
	printf("Digite uma frase para ser criptografada: ");
	fgets(frase, MAX, stdin);
	criptografar(frase_cripto, chave, frase);
	printf("A frase criptografada é: %s\n", frase_cripto);

	return 0;
}

void maiusculas(char chave[]);
void eliminar_extras(char chave[]);
void completar_chave(char chave[]);

void cria_chave(char chave[], char frase[]){
	strcpy(chave, frase);
	maiusculas(chave);
	eliminar_extras(chave);
	completar_chave(chave);
}

void maiusculas(char chave[]){
	for(int i=0; chave[i]!='\0'; i++){
		chave[i] = toupper(chave[i]);
	}
}
void comprime(char chave[], int c, int i);

void eliminar_extras(char chave[]){
	int c;
	
	for(int i = 0;(c = chave[i]) != '\0';){
		if(!isalpha(c) && c!= ' ' && c!='.' && c!=','){
			comprime(chave, c, i);
		} else {
			comprime(chave, c, i+1);
			i++;
		}
	}
}

void comprime(char s[], int c, int i){
	int j = i;
	
	while(s[i]!='\0'){
		if (s[i] != c){
			s[j++] = s[i];
		}
		i++;
	}
	s[j]='\0';
}

void completar_chave(char chave[]){
	char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ,. ";
	
	for(int i = 0; chave[i] != '\0'; i++){
		comprime(base, chave[i], 0);
	}
	
	strcat(chave, base);
}

int indice(int c);

void criptografar(char frase_cripto[], char chave[], char frase[]){
	
	int index_chave = 0;
	int c = 0;
	int index = 0;
	
	for (int i = 0; (c=toupper(frase[i]))!='\0'; i++){
		if((index = indice(c)) > 0){
			index_chave = (index_chave+index)%strlen(chave);
			frase_cripto[i] = chave[index_chave];
		} else {
			frase_cripto[i] = frase[i];
		}		
	}
	frase_cripto[strlen(frase)]='\0';
}

int indice(int c){
	char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ,. ";
	
	for (int i=0; base[i]!='\0'; i++){
		if (base[i] == c){
			return i+1;
		}
	}
	return 0;
}
