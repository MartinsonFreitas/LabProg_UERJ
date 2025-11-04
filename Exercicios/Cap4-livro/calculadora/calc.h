#include <stdio.h>
#include <stdlib.h> // para atof ()
#include <ctype.h>

#define NUMERO '0' // sinaliza que número foi encontrado

void push(double);
double pop(void);
int getop(char[]);
int getch(void);
void ungetch(int);
