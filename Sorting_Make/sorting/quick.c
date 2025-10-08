#include "sort.h"

void quick_sort(int* v, int inicio, int fim) {
    if (inicio >= fim) return;

    int pivo = v[(inicio + fim) / 2];
    int i = inicio;
    int j = fim;
    while (i <= j) {
        while (v[i] < pivo) i++;
        while (v[j] > pivo) j--;
        if (i <= j) {
            int tmp = v[i];
            v[i] = v[j];
            v[j] = tmp;
            i++;
            j--;
        }
    }
    if (inicio < j) quick_sort(v, inicio, j);
    if (i < fim) quick_sort(v, i, fim);
}
