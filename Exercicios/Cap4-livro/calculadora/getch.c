#include <stdio.h>
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
