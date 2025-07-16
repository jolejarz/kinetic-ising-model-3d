/**************************************************************
***************************************************************
*** 
***   data.h
***
***   Jason Olejarz
***   2010-07-21
***
***   This is the header file for data.c.
***
***************************************************************
**************************************************************/

struct parallel_index_data
{
	short int num;			// The number of data blocks to transfer.
	short int data_index[64];	// Each element is a data block to transfer.
	int data_size[64];		// Each element is the size (in bytes) of each data block that is transferred.
};

struct tag_data
{
	short int data_input[32];
	short int data_output[32];
};

void blinker_regions(struct evolve_parameters *parameters, void (*data[])(), void (*data_parameters[])(), short int data_size, short int data_index_flips, short int *spins, short int *output, int plot_steps);
void data_delete(void (*data[])(), void (*data_parameters[])(), short int data_index, short int data_size);
short int data_input(void (*data[])(), void (*data_parameters[])(), void **data_input, void **data_parameters_input, short int data_index, short int data_size);
short int data_output(void (*data[])(), void (*data_parameters[])(), void **data_output, void **data_parameters_output, int output_size, int output_parameters_size, short int data_index, short int data_size);
struct parallel_index_data parallel_index(short int command, short int data_index, int data_size, short int data_num);
void prob_dist(double *raw, double raw_min, double raw_max, int points, int raw_size, int bins, double *x, double *y);
void state(short int *spins, int *flips, int steps);
struct tag_data tag(short int command, short int data_input, short int data_output, short int data_num);

