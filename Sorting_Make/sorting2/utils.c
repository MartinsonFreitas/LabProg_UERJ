#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "utils.h"

void fill_random(int* v, int n, int max) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        v[i] = rand() % max;
    }
}

void print_array(int* v, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
}

int is_sorted(int* v, int n) {
    for (int i = 0; i < n - 1; i++) {
        if (v[i] > v[i+1]) {
            return 0;
        }
    }
    return 1;
}

