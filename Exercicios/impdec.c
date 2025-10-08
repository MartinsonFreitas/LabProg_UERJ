#include <stdio.h>

/* impdec: imprime n em decimal */
void impdec(int n)
{
	if (n < 0) {
		putchar ('-');
		n = -n;
	}
	
	if (n/10)
		impdec(n / 10);
	putchar (n % 10 + '0');
}


int main(void)
{
	impdec(-4138);
	return 0;
}

