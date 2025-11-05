#include <stdio.h>
#include "calc.h" // Necessário apenas para os protótipos se fossem usados em outro lugar

#define TAMBUF 100

char buf[TAMBUF]; // buffer para ungetch
int pbuf = 0; // próxima posição livre em buf

/* getch: obtém um caractere (talvez retornado) */
int getch(void) 
{
	return (pbuf > 0) ? buf[--pbuf] : getchar ();
}

/* ungetch: retorna caractere à entrada */
void ungetch(int c) 
{
	if (pbuf >= TAMBUF) {
		printf("ungetch: caracteres demais\n");
	} else {
		buf[pbuf++] = c;
	}
}
