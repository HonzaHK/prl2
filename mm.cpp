//VUTBR - FIT - PRL - project2
//Jan Kubis / xkubis13

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){

	int numprocs = 0; //pocet procesoru
	int myid = 0; //muj rank
	
	//MPI INIT
	MPI_Init(&argc,&argv); // inicializace MPI 
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs); // zjistíme, kolik procesů běží 
	MPI_Comm_rank(MPI_COMM_WORLD, &myid); // zjistíme id svého procesu 
	









	MPI_Finalize();
}