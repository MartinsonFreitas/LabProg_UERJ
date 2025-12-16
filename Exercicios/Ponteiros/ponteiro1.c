#include <stdio.h>

#define N sizeof(meu_array) / sizeof(meu_array[0])

int meu_array[] = {1, 23, 17, 4, -5, 100};
int *ptr;

int main(void) {
	ptr = &meu_array[0]; /* aponta nosso ponteiro para o primeiro
							elemento do array */
							
	for (int i = 0; i < N; i++) {
		printf("meu_array[%d] = %d\t", i, meu_array[i]); 	/*<-- A */
		//printf("ptr + %d = %d\n", i, *(ptr + i));			/*<-- B */
		//printf("ptr + %d = %d\n",i, *ptr++);
		printf("ptr + %d = %d\n",i, *(++ptr));
	}

	return 0;
}
