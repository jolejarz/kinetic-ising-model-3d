/**************************************************************
***************************************************************
*** 
***   ising3d.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This is the command line interface for ising3d.
***
***************************************************************
**************************************************************/

#include "includes.h"

// Set up the command arrays.
#include "commands.h"

#define loop_num 64	// Maximum number of functions in the preprocessing array
#define step_num 64	// Maximum number of functions in the step array
#define save_num 64	// Maximum number of functions in the postprocessing array

#define data_num 64	// Maximum number of data blocks

// Declare and initialize the function arrays.
void (*loop[loop_num])()={};
void (*step[step_num])()={};
void (*save[save_num])()={};

// Declare and initialize the data arrays.
void *data[data_num]={};
void *data_parameters[data_num]={};

// Declare the parameters array.
struct evolve_parameters parameters;

int main(int argc, char *argv[])
{
	// Initialize all values in 'parameters'.
	parameters.spins_initial=0;
	parameters.spins=0;
	parameters.flip_spin=0;
	parameters.flip=0;
	parameters.state=0;
	parameters.map=0;
	parameters.index=0;
	parameters.energy=0;
	parameters.mag=0;
	parameters.active=0;
	parameters.time=0;
	parameters.steps=0;
	parameters.bias=0;
	parameters.periodic_bc_x=0;
	parameters.periodic_bc_y=0;
	parameters.periodic_bc_z=0;
	parameters.nx=0;
	parameters.ny=0;
	parameters.nz=0;
	parameters.max_time=0;
	parameters.rng_seed=srand;	// Use the C compiler's random number generator functions.
	parameters.seed=0;
	parameters.rng=rand;		// Use the C compiler's random number generator functions.
	parameters.rng_seed_64=ising3d_rng_seed;
	parameters.seed_64=0;
	parameters.rng_64=ising3d_rng;
	parameters.rng_64_state=ising3d_rng_state;
	
	// The command that the user entered is stored here.
	char command[32]="";

//	int MPI_size, MPI_rank;
	
	int i, j;
	
	// Used for getting arguments from the user.
	int double_index=0, int_index=0, short_int_index=0, string_index=0;
	
	// Commands are input from this stream.
	FILE *commands;
	
	// If a file with a list of commands was passed as an argument, open it.
	if (argc==2)
	{
		commands=fopen(argv[1], "r");
	}
	// Otherwise, use standard input.
	else
	{
		commands=stdin;
	}
	
	// Initialize MPI.
//	MPI_Init (0, 0);
	
	// Get the number of MPI processes.
//	MPI_Comm_size (MPI_COMM_WORLD, &MPI_size);
	
	// Get this process's rank.
//	MPI_Comm_rank (MPI_COMM_WORLD, &MPI_rank);

	// A command file must be specified if there are multiple processes.
//	if (MPI_size>1 && commands==stdin)
//	{
//		if (MPI_rank==0) printf ("A command file must be specified if there are multiple processes.\n");
//		MPI_Finalize ();
//		return 0;
//	}
	
	// This loop processes commands from the user.
	while (1)
	{
		// Show the command prompt if no command file is being used.
		if (commands==stdin) {printf ("ising3d>> "); fflush (stdout);}
		
		// Input a command from the user.
		fscanf(commands, "%s", command);
		
		// Check if this is a valid command.
		for (i=0; i<sizeof(functions)/sizeof(void*); i++)
		{
			if (strcmp (command, cmd_name[i])==0)
			{
				// Check if the user entered "exit" or "quit".
				if (strcmp (cmd_name[i], "exit")==0 || strcmp (cmd_name[i], "quit")==0)
				{
//					MPI_Finalize ();
					return 0;
				}
				
				// Input any arguments from the user.
				for (j=0; j<num_args[i]; j++)
				{
					switch (arg_types[i][j])
					{
						case type_double: {fscanf(commands, "%lf", &arg_double[double_index++]); break;}
						case type_int: {fscanf(commands, "%d", &arg_int[int_index++]); break;}
						case type_short_int: {fscanf(commands, "%hd", &arg_short_int[short_int_index++]); break;}
						case type_string: fscanf(commands, "%s", arg_string[string_index++]);
					}
				}
				
				// Reset these variables.
				double_index=0, int_index=0; short_int_index=0, string_index=0;
				
				// Call the function corresponding to the entered command.
				functions[i] ();
				
				break;
			}
			
			// Check if an invalid command was entered.
			if (i==sizeof(functions)/sizeof(void*)-1)
			{
//				if (MPI_rank==0) printf ("Invalid command: %s.\n", command);
				printf ("Invalid command: %s.\n", command);
				
				// If commands are being read from a file, terminate.
				if (commands!=stdin)
				{
//					if (MPI_rank==0) printf ("Aborting.\n");
					printf ("Aborting.\n");
//					MPI_Finalize ();
					return 0;
				}
			}
		}
	}
}

// These files contain the code for all commands.
#include "cmd-clusters.c"
#include "cmd-data.c"
#include "cmd-file.c"
#include "cmd-list.c"
#include "cmd-loop.c"
#include "cmd-plot.c"
#include "cmd-run.c"
#include "cmd-save.c"
#include "cmd-set.c"
#include "cmd-step.c"

