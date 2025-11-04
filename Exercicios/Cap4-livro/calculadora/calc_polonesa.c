#include <stdio.h>
#include <stdlib.h> // para atof ()
#include <ctype.h>


#define MAXOP 100 // tamanho máximo operando ou operador
#define NUMERO '0' // sinaliza que número foi encontrado

int getop(char []);
void push(double);
double pop(void);

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

#define MAXVAL 100 	// profundidade max. da pilha
int pp = 0; 		// próxima posicão livre na pilha
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


int getch(void);
void ungetch(int);

/* getop: obtém próximo operador ou operando numérico */
int getop(char s[]){
	
	int i, c;

	while ((s[0] = c = getch()) == ' ' || c == '\t')
		;
		
	s[1] = '\0';
	
	if (!isdigit(c) && c != '.') {
		return c; 	// não é um número
	}
	
	i = 0;
	
	if (isdigit(c)) // agrupa parte inteira
	
		while (isdigit(s[++i] = c = getch ()))
			;
			
	if (c == '.') // agrupa parte fracionária
		while (isdigit(s[++i] = c = getch()))
			;
			
	s[i] = '\0';
	
	if (c != EOF) {
		ungetch(c);
	}
	
	return NUMERO;
}

#define TAMBUF 100

char buf[TAMBUF]; // buffer para ungetch
int pbuf = 0; // próxima posição livre em buf

int getch(void) // obtém um caractere (talvez retornado)
{
	return (pbuf > 0) ? buf[--pbuf] : getchar ();
}

void ungetch(int c) // retorna caractere à entrada
{
	if (pbuf >= TAMBUF) {
		printf("ungetch: caracteres demais\n");
	} else {
		buf[pbuf++] = c;
	}
}
