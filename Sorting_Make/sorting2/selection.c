#include "sort.h"

void selection_sort(int* v, int n) {
    int i, j, min_idx, temp;
    
    for (i = 0; i < n - 1; i++) {
		
        min_idx = i;
        
        for (j = i + 1; j < n; j++) {
			
            if (v[j] < v[min_idx]) {
                min_idx = j;
            }
        }
        
        temp = v[min_idx];
        v[min_idx] = v[i];
        v[i] = temp;
        
    }
}

