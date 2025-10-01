/*
 # Escreva uma versão alternativa de comprime(s1, s2) que remova cada caractere de s1 
 # que se case a algum caractere presente na string s2.
 # 
 ###  comprime: deleta todos os c de s 
 #
 # 	void comprime (char s[], int c)
 # 	{
 # 		int i,j;
 #
 #		for(i = j = 0; s[i] != '\0'; i++){
 #			if (s[i] != c){
 #				s[j++] = s[i];
 #			}
 #		}
 #		s[j] = '\0';
 #	}
 ### 
*/

#include <stdio.h>

//definição de constantes
#define TRUE 1
#define FALSE 0

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

int main(void) {
	
    char texto[] = "banana";
    
    comprime(texto, "an");  // remove todos os 'a' e 'n'
    printf("%s\n", texto);  // saída: "b"
    
    return 0;
}


