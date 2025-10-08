#include "sort.h"

void insertion_sort(int* v, int n) {
    int i, key, j;
    for (i = 1; i < n; i++) {
        key = v[i];
        j = i - 1;
        while (j >= 0 && v[j] > key) {
            v[j + 1] = v[j];
            j = j - 1;
        }
        v[j + 1] = key;
    }
}

