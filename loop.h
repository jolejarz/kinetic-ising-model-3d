/**************************************************************
***************************************************************
*** 
***   loop.h
***
***   Jason Olejarz
***   2010-07-21
***
***   This is the header file for loop.c.
***
***************************************************************
**************************************************************/

struct loop_iterate_data
{
	int count;
};

struct loop_randinit_data
{
	double init_mag;
};

struct loop_size_linear_data
{
	int min;
	int increment;
	int iterations;
};

struct loop_seed_data
{
	short int data_index;
	int num;
};

struct loop_seed_64_data
{
	short int data_index_low;
	short int data_index_high;
	int num;
};

short int loop_iterate(short int command, struct loop_iterate_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int loop_randinit(short int command, struct loop_randinit_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int loop_size_linear(short int command, struct loop_size_linear_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int loop_seed(short int command, struct loop_seed_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int loop_seed_64(short int command, struct loop_seed_64_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int loop_seed_tsc(short int command, void *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int loop_seed_tsc_64(short int command, void *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);

