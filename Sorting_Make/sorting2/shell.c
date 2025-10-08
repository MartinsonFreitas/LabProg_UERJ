#include "sort.h"

void shell_sort(int* v, int n){
	int inter, i, j, temp;

	for (inter = n / 2; inter > 0; inter /= 2)
		for (i = inter; i < n; i++)
			for(j = i - inter; j >= 0 && v[j] > v[j + inter]; j -= inter) {
				temp = v[j];
				v[j] = v[j + inter];
				v[j + inter] = temp;
	}
}

