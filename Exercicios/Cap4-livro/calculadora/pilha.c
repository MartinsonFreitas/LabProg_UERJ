#include <stdio.h>
#include "calc.h" // Para os protótipos de push/pop (embora sejam definidos aqui)

#define MAXVAL 100 	// profundidade max. da pilha
int pp = 0; 		// próxima posicão livre na pilha (Índice sp na imagem)
double val[MAXVAL]; // pilha de valores

/* push: empilha f na pilha de valores */
void push(double f){
	if (pp < MAXVAL) {
		val [pp++] = f;
	} else {
		printf ("Erro: pilha cheia %g\n", f);
	}
}

/* pop: retira e retorna valor do topo da pilha */
double pop(void){
	if (pp > 0) {
		return val [--pp];
	} else {
		printf ("Erro: pilha vazia\n");
		return 0.0;
	}
}
