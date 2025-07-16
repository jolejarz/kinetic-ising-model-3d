/**************************************************************
***************************************************************
*** 
***   calc.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains the routine used to obtain data.
***
***************************************************************
**************************************************************/

#include "includes.h"

void calc(short int (*loop[])(), short int (*step[])(), short int (*save[])(), short int loop_size, short int step_size, short int save_size, short int data_size, void *data[], void *data_parameters[], struct evolve_parameters *parameters)
{
	// If 'next' is set to one, then there is another iteration of the loop.
	int result, next=1;
	
	short int loop_index=0, save_index=0;
	
	// Send 'begin' to all functions in 'loop' and 'save'.
	while (loop[loop_index]!=0 && loop_index<loop_size)
	{
		if (loop[loop_index++](command_begin, 0, data, data_parameters, data_size, parameters)==result_error) goto calc_end;
	}
	loop_index=0;
	while (save[save_index]!=0 && save_index<save_size)
	{
		if (save[save_index++](command_begin, 0, data, data_parameters, data_size, parameters)==result_error) goto calc_end;
	}
	save_index=0;

	do
	{
		// Each function in 'loop' is for preprocessing.
		// 'evolve' is for running the simulation.
		// Each function in 'save' is for postprocessing.

		// Do preprocessing.
		while (loop[loop_index]!=0 && loop_index<loop_size)
		{
			result=loop[loop_index++](command_run, 0, data, data_parameters, data_size, parameters);
			if (result==result_error) goto calc_end;
			else if (result==result_done) next=0;
		}
		loop_index=0;

		// Test to make sure all parameters for evolve are valid.
		if (set_evolve(parameters)==set_error) goto calc_end;
		
		// Load the initial lattice.
		memcpy(parameters->spins, parameters->spins_initial, sizeof(short int)*(parameters->nx)*(parameters->ny)*(parameters->nz));
		
		// Run the simulation.
		if (evolve(	step,
				step_size,
				data,
				data_parameters,
				data_size,
				parameters,
				parameters->spins,
				&(parameters->flip_spin),
				&(parameters->flip),
				parameters->state,
				parameters->map,
				parameters->index,
				&(parameters->energy),
				&(parameters->mag),
				&(parameters->active),
				&(parameters->time),
				&(parameters->steps),
				parameters->bias,
				parameters->periodic_bc_x,
				parameters->periodic_bc_y,
				parameters->periodic_bc_z,
				parameters->nx,
				parameters->ny,
				parameters->nz,
				parameters->max_time,
				parameters->rng_seed,
				parameters->seed,
				parameters->rng,
				parameters->rng_seed_64,
				parameters->seed_64,
				parameters->rng_64,
				0,
				0)
			==result_error) goto calc_end;

		// Do postprocessing.
		while (save[save_index]!=0 && save_index<save_size)
		{
			result=save[save_index++](command_run, 0, data, data_parameters, data_size, parameters);
			if (result==result_error) goto calc_end;
			else if (result==result_skip) break;
		}
		save_index=0;
	}
	while (next);

	// Send 'end' to all functions in 'loop' and 'save'.
	while (loop[loop_index]!=0 && loop_index<loop_size)
	{
		if (loop[loop_index++](command_end, 0, data, data_parameters, data_size, parameters)==result_error) goto calc_end;
	}
	loop_index=0;
	while (save[save_index]!=0 && save_index<save_size)
	{
		if (save[save_index++](command_end, 0, data, data_parameters, data_size, parameters)==result_error) goto calc_end;
	}
	save_index=0;

	calc_end:
	
	return;
}

