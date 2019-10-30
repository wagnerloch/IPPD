#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

void primo (int numero);

int main(int argc, char const *argv[]) {

	int numeroMaximo = atoi(argv[1]);
	int numeroProcessos = omp_get_num_threads();
	int rankAtual = omp_get_thread_num();

	#pragma omp parallel for schedule (static)
	for (int i = 2; i <= numeroMaximo; i++) {
   		primo (i);
   	}

	return 0;
}

/**Função que imprime o número se e somente se ele for primo**/
void primo (int numero) {
	int divisivel = 0;
	for (int i = 2; i <= numero/2; i++) {
		if (numero % i == 0) {
			divisivel++;
			break;
		}
	}

	if (divisivel == 0) {
		printf("%d ", numero);
	}
	return;
}