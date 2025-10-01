#include<stdio.h>

/* contabits: conta bits 1 em x */
int contabits (unsigned x) {
	int b;

	for (b = 0; x != 0; x >>= 1) {
		if (x & 01) {
			b++;
			}
		}
		
	return b;
}

int main(void){
	printf("num bits 1 =%u\n", contabits(0xE));
	return 0;
}
