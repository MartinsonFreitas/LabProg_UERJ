/* pesqbin: acha x em v[0] <= v[1]... <= v[n - 1] */
int pesqbin (int x, int v[], int n)
{
	int inicio, fim, meio;
	inicio = 0;
	fim = n - 1;
	
	while (inicio <= fim) {
		meio = (inicio + fim) / 2;
	}
	
	if (x < v [meio]) {
		fim = meio - 1;
	} else if (x > v [meio]) {
		inicio = meio + 1;
	} else { // encontrou
		return meio;
	}
	return -1; //não encontrou
}
