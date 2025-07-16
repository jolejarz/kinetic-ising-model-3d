/**************************************************************
***************************************************************
*** 
***   data.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains functions for manipulating data.
***
***************************************************************
**************************************************************/

#include "includes.h"

void blinker_regions(struct evolve_parameters *parameters, void (*data[])(), void (*data_parameters[])(), short int data_size, short int data_index_flips, short int *spins, short int *output, int plot_steps)
{
	int nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);

	struct evolve_parameters temp_parameters;
	
	temp_parameters.state=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
	temp_parameters.map=(int*)malloc(sizeof(int)*nxtimesnytimesnz);
	temp_parameters.index=(int*)malloc(sizeof(int)*(1+nxtimesnytimesnz));
	
	temp_parameters.periodic_bc_x=parameters->periodic_bc_x;
	temp_parameters.periodic_bc_y=parameters->periodic_bc_y;
	temp_parameters.periodic_bc_z=parameters->periodic_bc_z;
	temp_parameters.nx=parameters->nx;
	temp_parameters.ny=parameters->ny;
	temp_parameters.nz=parameters->nz;
	temp_parameters.max_time=parameters->max_time;
	temp_parameters.rng_seed=parameters->rng_seed;
	temp_parameters.seed=parameters->seed;
	temp_parameters.rng=parameters->rng;
	temp_parameters.rng_seed_64=parameters->rng_seed_64;
	temp_parameters.seed_64=parameters->seed_64;
	temp_parameters.rng_64=parameters->rng_64;	

	short int *spins_test;
	
	if ((spins_test=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz))==0)
	{
		printf("Insufficient memory.\n");
		return;
	}
	
	memcpy(output, spins, sizeof(short int)*nxtimesnytimesnz);

	int *flips, *flips_size;

	if (plot_steps>0)
	{
		if (data_input(data, data_parameters, &flips, &flips_size, data_index_flips, data_size)==1) return;
		state(output, flips, plot_steps);
	}
	
	memcpy(spins_test, output, sizeof(short int)*nxtimesnytimesnz);
	
	if (
		evolve(	0,
			0,
			data,
			data_parameters,
			data_size,
			parameters,
			output,
			&(temp_parameters.flip_spin),
			&(temp_parameters.flip),
			temp_parameters.state,
			temp_parameters.map,
			temp_parameters.index,
			&(temp_parameters.energy),
			&(temp_parameters.mag),
			&(temp_parameters.active),
			&(temp_parameters.time),
			&(temp_parameters.steps),
			1,
			temp_parameters.periodic_bc_x,
			temp_parameters.periodic_bc_y,
			temp_parameters.periodic_bc_z,
			temp_parameters.nx,
			temp_parameters.ny,
			temp_parameters.nz,
			temp_parameters.max_time,
			temp_parameters.rng_seed,
			temp_parameters.seed,
			temp_parameters.rng,
			temp_parameters.rng_seed_64,
			temp_parameters.seed_64,
			temp_parameters.rng_64,
			1,
			0)
		==result_error
		||
		evolve(	0,
			0,
			data,
			data_parameters,
			data_size,
			parameters,
			spins_test,
			&(temp_parameters.flip_spin),
			&(temp_parameters.flip),
			temp_parameters.state,
			temp_parameters.map,
			temp_parameters.index,
			&(temp_parameters.energy),
			&(temp_parameters.mag),
			&(temp_parameters.active),
			&(temp_parameters.time),
			&(temp_parameters.steps),
			-1,
			temp_parameters.periodic_bc_x,
			temp_parameters.periodic_bc_y,
			temp_parameters.periodic_bc_z,
			temp_parameters.nx,
			temp_parameters.ny,
			temp_parameters.nz,
			temp_parameters.max_time,
			temp_parameters.rng_seed,
			temp_parameters.seed,
			temp_parameters.rng,
			temp_parameters.rng_seed_64,
			temp_parameters.seed_64,
			temp_parameters.rng_64,
			-1,
			0)
		==result_error
	)
	{
		printf("An error occurred while determining the blinking regions.\n");
		return;
	}

	int i;

	for (i=0; i<nxtimesnytimesnz; i++)
	{
		*(output+i)=(*(output+i)-*(spins_test+i))/2;
	}
	
	free(spins_test);
	
	free(temp_parameters.index);
	free(temp_parameters.map);
	free(temp_parameters.state);
	
	return;
}

void data_delete(void (*data[])(), void (*data_parameters[])(), short int data_index, short int data_size)
{
	if (data[data_index]!=0 && data_index<data_size)
	{
		free(data[data_index]); data[data_index]=0;
		free(data_parameters[data_index]); data_parameters[data_index]=0;
	}

	return;
}

short int data_input(void (*data[])(), void (*data_parameters[])(), void **data_input, void **data_parameters_input, short int data_index, short int data_size)
{
	if (data_index>=data_size)
	{
		printf("Data index must be between 0 and %hd.\n", data_size-1);
		return 1;
	}
	if (data[data_index]==0)
	{
		printf("Data index %hd is empty.\n", data_index);
		return 1;
	}

	*data_input=data[data_index];
	*data_parameters_input=data_parameters[data_index];
	
	return 0;
}

short int data_output(void (*data[])(), void (*data_parameters[])(), void **data_output, void **data_parameters_output, int output_size, int output_parameters_size, short int data_index, short int data_size)
{
	if (data_index>=data_size)
	{
		printf("Data index must be between 0 and %hd.\n", data_size-1);
		return 1;
	}
	if (data[data_index]!=0)
	{
		printf("Data index %hd is already occupied.\n", data_index);
		return 1;
	}

	*data_output=malloc(output_size);
	if (*data_output==0)
	{
		printf("Insufficient memory.\n");
		return 1;
	}
	
	*data_parameters_output=malloc(output_parameters_size);
	if (*data_parameters_output==0)
	{
		printf("Insufficient memory.\n");
		return 1;
	}

	data[data_index]=*data_output;
	data_parameters[data_index]=*data_parameters_output;
	
	return 0;
}

struct parallel_index_data parallel_index(short int command, short int data_index, int data_size, short int data_num)
{
	static struct parallel_index_data a;
	
	static int i;

	switch (command)
	{
		case 0:			// '0' means to delete all entries from the tag list
		{
			for (i=0; i<data_num; i++)
			{
				a.data_index[i]=0;
				a.data_size[i]=0;
			}
			i=0;
			
			a.num=i;
			
			return a;
		}
		case 1:			// '1' means to add an entry to the tag list
		{
			if (i==data_num)
			{
				printf("The parallel index array is already full.\n");
				return a;
			}
			
			a.data_index[i]=data_index;
			a.data_size[i]=data_size;
			i++;

			a.num=i;
			
			return a;
		}
		default: return a;	// Any other value just returns the tag list
	}
}

void prob_dist(double *raw, double raw_min, double raw_max, int points, int raw_size, int bins, double *x, double *y)
{
	int i;
	double a_width=(raw_max-raw_min)/(double)bins;

	int *a=(int*)malloc(sizeof(int)*bins);
	
	for (i=0; i<bins; i++)
	{
		*(a+i)=0;
	}
	for (i=0; i<points; i++)
	{
		if (*(raw+i)>=raw_min && *(raw+i)<raw_max) (*(a+(int)((*(raw+i)-raw_min)/a_width)))++;
		else if (*(raw+i)==raw_max) (*(a+bins-1))++;
	}
	for (i=0; i<bins; i++)
	{
		*(x+i)=(double)i*a_width+raw_min+a_width/2.0;
		*(y+i)=(double)(*(a+i))/(double)raw_size/a_width;
	}

	free(a);
	
	return;
}

void state(short int *spins, int *flips, int steps)
{
	int i;
	
	for (i=0; i<steps; i++)
	{
		if (*(flips+2*i+1)==1) *(spins+*(flips+2*i))=-*(spins+*(flips+2*i));
	}
	
	return;
}

struct tag_data tag(short int command, short int data_input, short int data_output, short int data_num)
{
	static struct tag_data a;
	
	static int i;
	
	switch (command)
	{
		case 0:			// '0' means to delete all entries from the tag list
		{
			for (i=0; i<data_num/2; i++)
			{
				a.data_input[i]=0;
				a.data_output[i]=0;
			}
			i=0;
			
			return a;
		}
		case 1:			// '1' means to add an entry to the tag list
		{
			if (i==data_num/2)
			{
				printf("The tag array is already full.\n");
				return a;
			}
			
			if (data_input==data_output)
			{
				printf("The input and output indices must be different.\n");
				return a;
			}
			
			a.data_input[i]=data_input;
			a.data_output[i]=data_output;
			i++;

			return a;
		}
		default: return a;	// Any other value just returns the tag list
	}
}

