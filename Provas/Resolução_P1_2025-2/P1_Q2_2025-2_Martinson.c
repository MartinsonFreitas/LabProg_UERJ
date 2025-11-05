#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


// tamanho máximo da mensagem
#define MAX_MENSAGEM 100

//definição de constantes
#define TRUE 1
#define FALSE 0

int valida_num(int n);
int primo(int n);
int somaQuadrados(int n);
int feliz(int n);
void maiusculas(char chave[]);

void encriptaCesar(char message[], int k);

// Função main
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
    
    // verifica se eh numero valido
    int eh_num_valido = valida_num(num);
    
    if (!eh_num_valido){
		printf("Não é numero valido. Fim do programa.\n");
        return 0; // Encerra se não for Primo Feliz
		}

    // Verificação de Primo Feliz
    int eh_primo = primo(num);
    int eh_feliz = feliz(num);
    int eh_primo_feliz = eh_primo && eh_feliz;
    
    if (!eh_primo_feliz) {
        printf("Não é primo feliz. Fim do programa.\n");
        return 0; // Encerra se não for Primo Feliz
    }
    
	// recebe frase para criptografar
    char frase[MAX_MENSAGEM];   
    
    // Cria Chave de Criptografia K
    int K = num % 26;
    
    // Leitura da mensagem de texto
    printf("%d é primo feliz! Chave K = %d\n", num,  K);
    printf("Digite a mensagem: ");
    
    // Leitura segura da string
    fgets(frase, MAX_MENSAGEM, stdin);
    
    // Remove o caractere '\n' - se existir
    frase[strcspn(frase, "\n")] = 0;
    
    // transforma maiuscula
    maiusculas(frase);
        
    // Cifra de César e impressão
    encriptaCesar(frase, K);
    
    printf("Mensagem Criptografada: %s\n", frase);
    
    return 0;
}

// verifica se o numero eh valido
int valida_num(int n){
	if (n <= 1 || n >10000) {
        return FALSE; // Números menores ou iguais a 1 não são primos.
    } else {
		return TRUE;
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
int somaQuadrados(int n) {
	
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
        n = somaQuadrados(n);
    }
    
    // Se o resultado final for 1, é feliz. Se for 4, é infeliz.
    return n == 1;
}

// coloca maiscula
void maiusculas(char chave[]){
	for(int i=0; chave[i]!='\0'; i++){
		chave[i] = toupper(chave[i]);
	}
}


// criptografia de César
void encriptaCesar(char mensagem[], int k) {
    int i = 0;
    
    while (mensagem[i] != '\0') {
        char caracter = mensagem[i];
        
        // Verifica se é uma letra maiúscula (A-Z)
        if (caracter >= 'A' && caracter <= 'Z') {
            // Converte o caractere para o índice 0-25
            int indice = caracter - 'A';
            
            // Aplica o deslocamento e o módulo (26) para circularidade
            int troca_indice = (indice + k) % 26;
            
            // Converte de volta para o caractere ASCII
            mensagem[i] = troca_indice + 'A';
        }
        // Outros caracteres (como espaços) são mantidos inalterados
        i++;
    }
}
