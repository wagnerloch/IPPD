#include <stdio.h>
#include <stdlib.h>
#include <time.h>       /* time_t, struct tm, time, localtime, strftime */
#include <mpi.h>

void primo (int numero);

int main(int argc, char *argv[]) {
	int numeroProcessos, rankAtual;
	int numeroMaximo = atoi(argv[1]);

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numeroProcessos);
   	MPI_Comm_rank(MPI_COMM_WORLD, &rankAtual);

   	for (int i = 2 + rankAtual; i <= numeroMaximo; i = i + numeroProcessos) {
   		primo (i);
   	}

   	MPI_Finalize();
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