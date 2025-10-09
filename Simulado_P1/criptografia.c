#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define TAM_MAX_FRASE 256
#define TAM_CHAVE 29 // A-Z (26) + ',' (1) + '.' (1) + ' ' (1)

// Protótipos das Funções
void func_maiscula(char *str);
int obter_val_caract(char c);
void gerar_chave(char *frase, char *chave);
void encriptar_frase(char *frase, char *chave, char *frase_encriptada);

// Chamada da função main
int main() {
    char ent_frase_chave[TAM_MAX_FRASE];
    char chave_gerada[TAM_CHAVE + 1];
    char frase_para_criptografar[TAM_MAX_FRASE];
    char saida_criptografada[TAM_MAX_FRASE];

    printf("\n\tDigite a frase para gerar a chave criptográfica: ");
    fgets(ent_frase_chave, TAM_MAX_FRASE, stdin);
    ent_frase_chave[strcspn(ent_frase_chave, "\n")] = '\0'; // Remove "\n"

    gerar_chave(ent_frase_chave, chave_gerada);
    printf("\tChave gerada: \"%s\"\n", chave_gerada);

    printf("\n\tDigite a frase a ser criptografada: ");
    fgets(frase_para_criptografar, TAM_MAX_FRASE, stdin);
    frase_para_criptografar[strcspn(frase_para_criptografar, "\n")] = '\0'; // Remove "\n"

    encriptar_frase(frase_para_criptografar, chave_gerada, saida_criptografada);
    printf("\tFrase criptografada: \"%s\"\n\n", saida_criptografada);    

    return 0;
}

// Converte todas as letras de uma string para maiúsculas
void func_maiscula(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper(str[i]);
    }
}

// Função para obter o valor de um caractere
int obter_val_caract(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 1;
    } else if (c == ',') {
        return 27;
    } else if (c == '.') {
        return 28;
    } else if (c == ' ') {
        return 29;
    }
    return 0; // retorna se caractere inválido
}

// Gera a chave criptográfica a partir de uma frase
void gerar_chave(char *frase, char *chave) {
    char frase_temp[TAM_MAX_FRASE];
    strcpy(frase_temp, frase);
    func_maiscula(frase_temp);

    char caracter[TAM_CHAVE + 1];
    int contador = 0;
    int ja_tem_caracter[256] = {0}; // Para marcar caracteres já vistos

    // Processa a frase para obter caracteres únicos e válidos
    for (int i = 0; frase_temp[i] != '\0'; i++) {
        char c = frase_temp[i];
        if ((c >= 'A' && c <= 'Z') || c == ',' || c == '.' || c == ' ') {
            if (!ja_tem_caracter[(int)c]) {
                caracter[contador++] = c;
                ja_tem_caracter[(int)c] = 1;
            }
        }
    }
    caracter[contador] = '\0';

    // Adiciona caracteres que faltam a chave
    char caracteres_validos[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ,. ";
    for (int i = 0; caracteres_validos[i] != '\0'; i++) {
        char c = caracteres_validos[i];
        if (!ja_tem_caracter[(int)c]) {
            caracter[contador++] = c;
            ja_tem_caracter[(int)c] = 1;
        }
    }
    caracter[contador] = '\0';

    strcpy(chave, caracter);
}

// Criptografa uma frase usando a chave fornecida
void encriptar_frase(char *frase, char *chave, char *frase_encriptada) {
    char frase_temp[TAM_MAX_FRASE];
    strcpy(frase_temp, frase);
    func_maiscula(frase_temp);

    int tam_chave = strlen(chave);
    int indice_chave = 0;
    int indice_encriptado = 0;

    for (int i = 0; frase_temp[i] != '\0'; i++) {
        char caracter_para_encriptar = frase_temp[i];
        int valor_caracter = obter_val_caract(caracter_para_encriptar);

        if (valor_caracter == 0) { // Caractere invalido, apenas copia
            frase_encriptada[indice_encriptado++] = caracter_para_encriptar;
        } else {
            indice_chave = (indice_chave + valor_caracter) % tam_chave;
            frase_encriptada[indice_encriptado++] = chave[indice_chave];
        }
    }
    frase_encriptada[indice_encriptado] = '\0';
}
