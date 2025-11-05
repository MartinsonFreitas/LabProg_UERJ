#include <stdio.h>
#include <stdlib.h> // Para a função atof() padrão
#include "calc.h"   // Inclui protótipos e NUMERO

#define MAXOP 100 // tamanho máximo operando ou operador

/* calculadora polonesa reversa */
int main (){
    
    int tipo;
    double op2;
    char s[MAXOP];
    
    while ((tipo = getop(s)) != EOF) {
        switch (tipo) {
        case NUMERO:
            push(atof(s));
            break;
            
        case '+':
            push(pop() + pop());
            break;
        
        // ... (o restante do seu switch, incluindo *, -, /, \n, default) ...
        
        case '*':
            push(pop () * pop());
            break;
        
        case '-':
            op2 = pop();
            push (pop() - op2);
            break;
        
        case '/':
            op2 = pop();
            
            if (op2 != 0.0) {
                push(pop() / op2);
            } else {
                printf ("erro: divisor zero\n");
            }
            
            break;
            
        case '\n':
            printf("\t%.8g\n", pop());
            break;
        
        default:
            printf ("Erro: comando desconhecido %s\n", s);
            break;
        }
    }
    
    return 0;
}

// comando no terminal para compilar tudo!
// gcc main.c pilha.c getop.c getch.c -o calc
