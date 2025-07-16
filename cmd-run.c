/**************************************************************
***************************************************************
*** 
***   cmd-run.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains the command for running the simulation.
***
***************************************************************
**************************************************************/

void cmd_run()
{
	// If this is the master process, do not do any calculation.
//	int rank;
//	int size;
//	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//	MPI_Comm_size(MPI_COMM_WORLD, &size);
//	if (rank==0 && size>1) return;
	
	calc(loop, step, save, loop_num, step_num, save_num, data_num, data, data_parameters, &parameters);
	return;
}

