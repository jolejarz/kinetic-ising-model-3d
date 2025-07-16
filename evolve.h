/**************************************************************
***************************************************************
*** 
***   evolve.h
***
***   Jason Olejarz
***   2010-07-21
***
***   This is the header file for evolve.c.
***
***************************************************************
**************************************************************/

struct evolve_parameters
{
	short int *spins_initial;
	short int *spins;
	int flip_spin;
	int flip;
	short int *state;
	int *map;
	int *index;
	int energy;
	int mag;
	int active;
	double time;
	int steps;
	short int bias;
	short int periodic_bc_x;
	short int periodic_bc_y;
	short int periodic_bc_z;
	int nx;
	int ny;
	int nz;
	double max_time;
	int (*rng_seed)();
	int seed;
	int (*rng)();
	void (*rng_seed_64)();
	unsigned long int seed_64;
	unsigned long int (*rng_64)();
	unsigned long int (*rng_64_state)();
	short int altbias;
};

short int evolve(short int (*step[])(), short int step_size, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters, short int *spins, int *flip_spin, int *flip, short int *state, int *map, int *index, int *energy, int *mag, int *active, double *time, int *steps, short int bias, short int periodic_bc_x, short int periodic_bc_y, short int periodic_bc_z, int nx, int ny, int nz, double max_time, int (*rng_seed)(), int seed, int (*rng)(), void (*rng_seed_64)(), unsigned long int seed_64, unsigned long int (*rng_64)(), short int force_flips, int *num_energy_drops);

