# OpenMPI

Compilação:

	mpicc prog.c -o prog -lm
    mpicc prog_mpi.c -o prog_mpi  
    ou 
    mpic++ prog_mpi.cpp -o prog_mpi

    mpirun -np <numProcessos> ./prog_mpi