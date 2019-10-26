#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void merge(int vetor[], int comeco, int meio, int fim) {
    int com1 = comeco, com2 = meio+1, comAux = 0, tam = fim-comeco+1;
    int *vetAux;
    vetAux = (int*)malloc(tam * sizeof(int));

    while(com1 <= meio && com2 <= fim){
        if(vetor[com1] < vetor[com2]) {
            vetAux[comAux] = vetor[com1];
            com1++;
        } else {
            vetAux[comAux] = vetor[com2];
            com2++;
        }
        comAux++;
    }

    while(com1 <= meio){  //Caso ainda haja elementos na primeira metade
        vetAux[comAux] = vetor[com1];
        comAux++;
        com1++;
    }

    while(com2 <= fim) {   //Caso ainda haja elementos na segunda metade
        vetAux[comAux] = vetor[com2];
        comAux++;
        com2++;
    }

    for(comAux = comeco; comAux <= fim; comAux++){    //Move os elementos de volta para o vetor original
        vetor[comAux] = vetAux[comAux-comeco];
    }

    free(vetAux);
}

void mergeSort(int vetor[], int comeco, int fim){
    if (comeco < fim) {
        int meio = (fim+comeco)/2;

        mergeSort(vetor, comeco, meio);
        mergeSort(vetor, meio+1, fim);
        merge(vetor, comeco, meio, fim);
    }
}

void main(int argc, char** argv){

  time_t t;
  int v_size = atoi(argv[1]);
  int *v = malloc(v_size * sizeof(int));
  // Initialize the MPI environment
  MPI_Init(NULL, NULL);

  double tbeg = MPI_Wtime();

  // Find out rank, size
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int array_chunk_size = v_size / world_size;

  if(world_rank == 0){
    //initialize array
    printf("!\n");
    printf("world_rank:%d\nworld size:%d\n",world_rank, world_size );
    printf("Tamanho do vetor: %d\n",v_size);
    printf("Tamanho do Chunk: %d\n", array_chunk_size);
    srand((unsigned) time(&t));
    for(int i = 0; i < v_size; i++){
        v[i] = rand()%1001;
    }  
  }

  int *sub_array = malloc(array_chunk_size * sizeof(int));

  MPI_Scatter(v, array_chunk_size, MPI_INT, sub_array,
            array_chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

  mergeSort(sub_array,0,(array_chunk_size-1));

  //sorted array to gather and merge
  int *sorted = NULL;
  if(world_rank == 0) {
    sorted = malloc(v_size * sizeof(int));
    }

  //unifing all sub_array into one
  MPI_Gather(sub_array, array_chunk_size, MPI_INT, sorted, array_chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

  //final merge
  
  if(world_rank == 0){
    mergeSort(sorted,0,v_size-1);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  double elapsedTime = MPI_Wtime() - tbeg;
  
  printf("Processo:%d\nTempo no processo:%f\n",world_rank,elapsedTime);
  
  double totalTime;
  MPI_Reduce( &elapsedTime, &totalTime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD );
  
  if (world_rank == 0 ) {
      printf("Tempo total acumulado:%f\n", totalTime );
  }

  free(sorted);
  free(v);
  free(sub_array);
  MPI_Finalize();

}