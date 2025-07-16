/**************************************************************
***************************************************************
*** 
***   set.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains the routines used to set the parameters for 'evolve'.
***
***************************************************************
**************************************************************/

#include "includes.h"

short int set_altbias(short int simulation_type, struct evolve_parameters *parameters)
{
	if (simulation_type!=0 && simulation_type!=1 && simulation_type!=2)
	{
		printf("The setting for the infinitesimal bias field used to force the system into the final state\n");
		printf("must be either 'none' (0), 'alternate after each drop in energy' (1), or 'choose randomly after\n");
		printf("each drop in energy' (2).\n");
		return set_error;
	}

	parameters->altbias=simulation_type;

	return set_ok;
}

short int set_antiferro(short int firstspin, struct evolve_parameters *parameters)
{
	int i, nx, ny, nz, nxtimesny, nxtimesnytimesnz;
	
	nx=parameters->nx; ny=parameters->ny; nz=parameters->nz;
	nxtimesny=nx*ny; nxtimesnytimesnz=nxtimesny*nz;

	*(parameters->spins_initial)=firstspin;
			
	for (i=1; i<nxtimesnytimesnz; i++)
	{
		*(parameters->spins_initial+i)=-(*(parameters->spins_initial+i-1));

		if (i%nxtimesny==0)
		{
			if (nx%2==1 && ny%2==0 || nx%2==0 && ny%2==1) *(parameters->spins_initial+i)=-(*(parameters->spins_initial+i));
		}
		else if (i%nx==0 && nx%2==0) *(parameters->spins_initial+i)=-(*(parameters->spins_initial+i));
	}

	return set_ok;
}

short int set_bc(short int periodic_bc_x, short int periodic_bc_y, short int periodic_bc_z, struct evolve_parameters *parameters)
{
	if (	(periodic_bc_x!=0 && periodic_bc_x!=1) ||
		(periodic_bc_y!=0 && periodic_bc_y!=1) ||
		(periodic_bc_z!=0 && periodic_bc_z!=1))
	{
		printf("Boundary conditions must be either free (0) or periodic (1).\n");
		return set_error;
	}
	
	parameters->periodic_bc_x=periodic_bc_x;
	parameters->periodic_bc_y=periodic_bc_y;
	parameters->periodic_bc_z=periodic_bc_z;
	
	return set_ok;
}

short int set_bias(short int bias, struct evolve_parameters *parameters)
{
	if (bias!=0 && bias!=1 && bias!=-1)
	{
		printf("Bias field is either off (0) or on (1 or -1).\n");
		return set_error;
	}
	
	parameters->bias=bias;

	return set_ok;
}

short int set_block(short int spin, int xi, int xf, int yi, int yf, int zi, int zf, struct evolve_parameters *parameters)
{
	if (xi<1 || xf>parameters->nx || yi<1 || yf>parameters->ny || zi<1 || zf>parameters->nz)
	{
		printf("Invalid coordinates for the block.\n");
		return set_error;
	}
	
	int x, y, z, nxtimesny;
	
	nxtimesny=(parameters->nx)*(parameters->ny);
	
	for (z=zi; z<=zf; z++)
	{
		for (y=yi; y<=yf; y++)
		{
			for (x=xi; x<=xf; x++)
			{
				*(parameters->spins_initial+(x-1)+(y-1)*(parameters->nx)+(z-1)*nxtimesny)=spin;
			}
		}
	}
	
	return set_ok;
}

short int set_evolve(struct evolve_parameters *parameters)
{
	if (	(parameters->periodic_bc_x!=0 && parameters->periodic_bc_x!=1) ||
		(parameters->periodic_bc_y!=0 && parameters->periodic_bc_y!=1) ||
		(parameters->periodic_bc_z!=0 && parameters->periodic_bc_z!=1))
	{
		printf("Boundary conditions must be either free (0) or periodic (1).\n");
		return set_error;
	}

	if (parameters->bias!=0 && parameters->bias!=1 && parameters->bias!=-1)
	{
		printf("Bias field is either off (0) or on (1 or -1).\n");
		return set_error;
	}

	if (parameters->nx<=0 || parameters->ny<=0 || parameters->nz<=0)
	{
		printf("nx, ny, and nz must all be positive.\n");
		return set_error;
	}

	if (parameters->max_time<0)
	{
		printf("The maximum time to run the simulation must be nonnegative.\n");
		return set_error;
	}

	return set_ok;
}

short int set_randlat(double init_mag, struct evolve_parameters *parameters)
{
	long double largest_random_number_plus_1;
	
	// The largest random number that can be generated plus 1 is 2^64.
	largest_random_number_plus_1=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);

	int size=(parameters->nx)*(parameters->ny)*(parameters->nz);
	
	int *index=(int*)malloc(sizeof(int)*size);
	int index_size;
	
	int i,j;
	short int initial_spin;
	int num_flips;

	// Initialize the random number generator.
	parameters->rng_seed_64 (parameters->seed_64);
	
	initial_spin=init_mag>=0 ? 1 : -1;
	
	for (i=0; i<size; i++)
	{
		*(parameters->spins_initial+i)=initial_spin;
	}
	
	for (i=0; i<size; i++)
	{
		*(index+i)=i;
	}

	index_size=size;

	num_flips=(int)(size/2*(1-sqrt(init_mag*init_mag)));

	for (i=0; i<num_flips; i++)
	{
		j=(int)(index_size*((unsigned long int)(parameters->rng_64())/largest_random_number_plus_1));
		
		*(parameters->spins_initial+(*(index+j)))=-*(parameters->spins_initial+(*(index+j)));
		
		*(index+j)=*(index+index_size-1);

		index_size--;
	}

	return set_ok;
}

short int set_seed(int seed, struct evolve_parameters *parameters)
{
	parameters->seed=seed;
	return set_ok;
}

short int set_seed_64(double seed_low, double seed_high, struct evolve_parameters *parameters)
{
	parameters->seed_64=(unsigned long int)seed_high;
	parameters->seed_64*=(unsigned long int)pow(2,16);
	parameters->seed_64*=(unsigned long int)pow(2,16);
	parameters->seed_64+=(unsigned long int)seed_low;
	return set_ok;
}

short int set_settsc(struct evolve_parameters *parameters)
{
	asm ("rdtsc" : "=a" (parameters->seed) : : "%edx");
	parameters->rng_seed(parameters->seed);
	
	return set_ok;
}

short int set_size(int nx, int ny, int nz, struct evolve_parameters *parameters)
{
	if (nx<=0 || ny<=0 || nz<=0)
	{
		printf("nx, ny, and nz must all be positive.\n");
		return set_error;
	}
	
	int size=nx*ny*nz;
	
	if (parameters->spins_initial!=0) {free(parameters->spins_initial); parameters->spins_initial=0;}
	if (parameters->spins!=0) {free(parameters->spins); parameters->spins=0;}
	if (parameters->state!=0) {free(parameters->state); parameters->state=0;}
	if (parameters->map!=0) {free(parameters->map); parameters->map=0;}
	if (parameters->index!=0) {free(parameters->index); parameters->index=0;}
	
	int error=0;
	
	if ((parameters->spins_initial=(short int*)malloc(sizeof(short int)*size))==0) error=1;
	if ((parameters->spins=(short int*)malloc(sizeof(short int)*size))==0) error=1;
	if ((parameters->state=(short int*)malloc(sizeof(short int)*size))==0) error=1;
	if ((parameters->map=(int*)malloc(sizeof(int)*size))==0) error=1;
	if ((parameters->index=(int*)malloc(sizeof(int)*(1+size)))==0) error=1;
	
	if (error==1)
	{
		printf("Insufficient memory.\n");
		return set_error;
	}

	parameters->nx=nx;
	parameters->ny=ny;
	parameters->nz=nz;

	return set_ok;
}

short int set_size_realloc(int nx, int ny, int nz, struct evolve_parameters *parameters)
{
	if (nx<=0 || ny<=0 || nz<=0)
	{
		printf("nx, ny, and nz must all be positive.\n");
		return set_error;
	}
	
	int size=nx*ny*nz;
	
	int error=0;
	
	if ((parameters->spins_initial=(short int*)realloc(parameters->spins_initial, sizeof(short int)*size))==0) error=1;
	if ((parameters->spins=(short int*)realloc(parameters->spins, sizeof(short int)*size))==0) error=1;
	if ((parameters->state=(short int*)realloc(parameters->state, sizeof(short int)*size))==0) error=1;
	if ((parameters->map=(int*)realloc(parameters->map, sizeof(int)*size))==0) error=1;
	if ((parameters->index=(int*)realloc(parameters->index, sizeof(int)*(1+size)))==0) error=1;
	
	if (error==1)
	{
		printf("Insufficient memory.\n");
		return set_error;
	}

	parameters->nx=nx;
	parameters->ny=ny;
	parameters->nz=nz;

	return set_ok;
}

short int set_time(double time, struct evolve_parameters *parameters)
{
	if (time<0)
	{
		printf("The maximum time to run the simulation must be nonnegative.\n");
		return set_error;
	}

	parameters->max_time=time;
	return set_ok;
}

