#ifndef CALC_H
#define CALC_H

#define NUMERO '0'  // Sinaliza que número foi encontrado

// Funções da Pilha (pilha.c)
void push(double);
double pop(void);

// Funções de Entrada/Saída (getch.c)
int getch(void);
void ungetch(int);

// Função de Análise de Entrada (getop.c)
int getop(char []);

// Usaremos a atof() da stdlib.h (como ajustamos antes), 
// então não precisa do protótipo aqui, mas ele estaria aqui 
// se fosse implementado separadamente.

#endif /* CALC_H */
