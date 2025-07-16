/**************************************************************
***************************************************************
*** 
***   set.h
***
***   Jason Olejarz
***   2010-07-21
***
***   This is the header file for set.c.
***
***************************************************************
**************************************************************/

enum {set_error, set_ok};

short int set_altbias(short int simulation_type, struct evolve_parameters *parameters);
short int set_antiferro(short int firstspin, struct evolve_parameters *parameters);
short int set_bc(short int periodic_bc_x, short int periodic_bc_y, short int periodic_bc_z, struct evolve_parameters *parameters);
short int set_bias(short int bias, struct evolve_parameters *parameters);
short int set_block(short int spin, int xi, int xf, int yi, int yf, int zi, int zf, struct evolve_parameters *parameters);
short int set_check(short int check, double interval, struct evolve_parameters *parameters);
short int set_evolve(struct evolve_parameters *parameters);
short int set_randlat(double init_mag, struct evolve_parameters *parameters);
short int set_seed(int seed, struct evolve_parameters *parameters);
short int set_seed_64(double seed_low, double seed_high, struct evolve_parameters *parameters);
short int set_settsc(struct evolve_parameters *parameters);
short int set_size(int nx, int ny, int nz, struct evolve_parameters *parameters);
short int set_size_realloc(int nx, int ny, int nz, struct evolve_parameters *parameters);
short int set_steps(int steps, struct evolve_parameters *parameters);

