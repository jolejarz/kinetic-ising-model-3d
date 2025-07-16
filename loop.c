/**************************************************************
***************************************************************
*** 
***   loop.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains the routines used by 'calc' to set up the lattice before each run of 'evolve'.
***
***************************************************************
**************************************************************/

#include "includes.h"

short int loop_iterate(short int command, struct loop_iterate_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int i, count;
	static double last_percent, new_percent;
	
	switch (command)
	{
		case command_end: {printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bDone                    \n"); return result_continue;}
		case command_info: {printf("loop_iterate\n"); return result_continue;}
		case command_insert: {count=command_data->count; return result_continue;}
		case command_begin:
		{
			i=0;

			printf("Processing...      ");

			last_percent=-1;
			
			return result_continue;
		}
		case command_run:
		{
			new_percent=100*i/(double)count;
			
			if (new_percent>last_percent)
			{
				printf("\b\b\b\b\b");
				printf("%4.1lf%%", new_percent);
				last_percent=new_percent;
				fflush(stdout);
			}

			if (++i==count) return result_done;
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int loop_randinit(short int command, struct loop_randinit_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static double init_mag;
	
	switch (command)
	{
		case command_info: {printf("loop_randinit\n"); return result_continue;}
		case command_insert: {init_mag=command_data->init_mag; return result_continue;}
		case command_run:
		{
			set_randlat(init_mag, parameters);
			return result_continue;
		}
		default: return result_continue;
	}
}

short int loop_size_linear(short int command, struct loop_size_linear_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int min, increment, iterations;
	static int size, max, i;
	
	switch (command)
	{
		case command_info: {printf("loop_size_linear\n"); return result_continue;}
		case command_insert:
		{
			min=command_data->min;
			increment=command_data->increment;
			iterations=command_data->iterations;
			
			return result_continue;
		}
		case command_begin:
		{
			size=min;
			i=-1;

			if (set_size(size, size, size, parameters)==set_error)
			{
				printf("\nloop_size_linear: Insufficient memory.\n");
				return result_error;
			}
			
			return result_continue;
		}
		case command_run:
		{
			if (++i==iterations)
			{				
				i=0;
				size+=increment;
				if (set_size_realloc(size, size, size, parameters)==set_error)
				{
					printf("\nloop_size_linear: Insufficient memory.\n");
					return result_error;
				}
			}

			return result_continue;
		}
		default: return result_continue;
	}
}

short int loop_seed(short int command, struct loop_seed_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static short int data_index;
	static int num;
	
	static double *seed;
	static int *seed_size;

	static int i;

//	static int rank, size;
	
	switch (command)
	{
		case command_info: {printf("loop_seed\n"); return result_continue;}
		case command_insert:
		{
			data_index=command_data->data_index;
			num=command_data->num;
			return result_continue;
		}
		case command_begin:
		{
			if (data_input(data, data_parameters, &seed, &seed_size, data_index, data_size)==1) return result_error;

			// Get this process's rank.
//			MPI_Comm_rank(MPI_COMM_WORLD, &rank);

			// Get the total number of processes.
//			MPI_Comm_size(MPI_COMM_WORLD, &size);
			
			// Set the index into the list of seeds.
			// This ensures that processes of the same rank always get the same seeds.
			//
			// If this is the only process, then the list of seeds must be read
			// starting at the beginning.
			//
//			if (size==1) i=0;
//			else i=(rank-1)*num;
			i=0;
			
			return result_continue;
		}
		case command_run:
		{
			parameters->seed=(int)(*(seed+(i++)));
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int loop_seed_64(short int command, struct loop_seed_64_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static short int data_index_low;
	static short int data_index_high;
	static int num;
	
	static double *seed_64_low;
	static double *seed_64_high;
	static int *seed_64_size;

	static int i;

//	static int rank, size;
	
	switch (command)
	{
		case command_info: {printf("loop_seed_64\n"); return result_continue;}
		case command_insert:
		{
			data_index_low=command_data->data_index_low;
			data_index_high=command_data->data_index_high;
			num=command_data->num;
			return result_continue;
		}
		case command_begin:
		{
			if (data_input(data, data_parameters, &seed_64_low, &seed_64_size, data_index_low, data_size)==1) return result_error;
			if (data_input(data, data_parameters, &seed_64_high, &seed_64_size, data_index_high, data_size)==1) return result_error;

			// Get this process's rank.
//			MPI_Comm_rank(MPI_COMM_WORLD, &rank);

			// Get the total number of processes.
//			MPI_Comm_size(MPI_COMM_WORLD, &size);
			
			// Set the index into the list of seeds.
			// This ensures that processes of the same rank always get the same seeds.
			//
			// If this is the only process, then the list of seeds must be read
			// starting at the beginning.
			//
//			if (size==1) i=0;
//			else i=(rank-1)*num;
			i=0;
			
			return result_continue;
		}
		case command_run:
		{
			parameters->seed_64=(unsigned long int)(*(seed_64_high+i));
			parameters->seed_64*=(unsigned long int)pow(2,16);
			parameters->seed_64*=(unsigned long int)pow(2,16);
			parameters->seed_64+=(unsigned long int)(*(seed_64_low+i));
			i++;
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int loop_seed_tsc(short int command, void *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	switch (command)
	{
		case command_info: {printf("loop_seed_tsc\n"); return result_continue;}
		case command_run:
		{
			asm ("rdtsc" : "=a" (parameters->seed) : : "%edx");
			return result_continue;
		}
		default: return result_continue;
	}
}

short int loop_seed_tsc_64(short int command, void *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int low, high;
	
	switch (command)
	{
		case command_info: {printf("loop_seed_tsc_64\n"); return result_continue;}
		case command_run:
		{
			asm ("rdtsc" : "=a" (low), "=d" (high));
			parameters->seed_64=(unsigned long int)high;
			parameters->seed_64*=(unsigned long int)pow(2,16);
			parameters->seed_64*=(unsigned long int)pow(2,16);
			parameters->seed_64+=(unsigned long int)low;
			return result_continue;
		}
		default: return result_continue;
	}
}

