/*
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

//definição de constantes
#define MAX 100
#define TRUE 1
#define FALSE 0

int primo_feliz(int n);

void cria_chave(char chave[], char frase[]);
void criptografar(char frase_cripto[], char chave[], char frase[]);

int main() {
		
	// Verifica se eh primo...
    int num;
    char numero[20]; // Buffer para armazenar a entrada como string
    
    // --- Leitura e conversão do valor de entrada (fgets + atoi) ---
    printf("Digite um numero: ");
    if (fgets(numero, sizeof(numero), stdin) != NULL) 
    {
        // Converte a string lida para inteiro
        num = atoi(numero);
    } 
    else 
    {
        printf("Erro na leitura do numero.\n");
    }
    
    if(!primo_feliz(num){
		return 0;
	}

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

int primo(int n);
int soma_quadrados(int n);
int feliz(int n);

int primo_feliz(int n){
	 // Verificação de Primo Feliz
    int eh_primo = primo(n);
    int eh_feliz = feliz(n);
    int eh_primo_feliz = eh_primo && eh_feliz;
    
    if (!eh_primo_feliz){
        //printf("Não é primo feliz. Fim do programa.\n");
        return 0; // Encerra se não for Primo Feliz
    } 
}

// verifica se eh primo
int primo(int n) {
    if (n <= 1) {
        return FALSE; // Números menores ou iguais a 1 não são primos.
    }
    if (n <= 3) {
        return TRUE; // 2 e 3 são primos.
    }
    // Otimização: Ignora múltiplos de 2 e 3, verificando a partir de 5.
    if (n % 2 == 0 || n % 3 == 0) {
        return FALSE;
    }
    
    // Verifica divisores da forma 6k ± 1 até a raiz quadrada de n.
    for (int i = 5; (long long)i * i <= n; i = i + 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return FALSE;
        }
    }
    return TRUE;
}

// soma dos quadrados dos digitos
int soma_quadrados(int n) {
	
    int sum = 0;
    int digit;
    
    while (n > 0) {
        digit = n % 10;
        sum += digit * digit;
        n /= 10;
    }
    return sum;
}

// verifica se eh feliz
int feliz(int n) {
       
    // O loop continua até que o número se torne 1 (feliz) ou 4 (infeliz/ciclo).
    while (n != 1 && n != 4) {
        n = soma_quadrados(n);
    }
    
    // Se o resultado final for 1, é feliz. Se for 4, é infeliz.
    return n == 1;
}

void maiusculas(char chave[]);
//void eliminar_extras(char chave[]);
//void completar_chave(char chave[]);

void cria_chave(char chave[], char frase[]){
	strcpy(chave, frase);
	maiusculas(chave);
	//eliminar_extras(chave);
	//completar_chave(chave);
}

void maiusculas(char chave[]){
	for(int i=0; chave[i]!='\0'; i++){
		chave[i] = toupper(chave[i]);
	}
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
