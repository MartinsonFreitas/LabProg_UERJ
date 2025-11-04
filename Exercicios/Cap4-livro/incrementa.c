#include <stdio.h>

int main(){
		int x = 5;
		int y = x++;
		int z = x++;
				
		printf("x = %d, y = %d, z = %d\n", x, y, z);
		//printf("x = %d, y = %d\n", x, y);
		return 0;
}
