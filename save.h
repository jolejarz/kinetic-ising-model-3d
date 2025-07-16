/**************************************************************
***************************************************************
*** 
***   save.h
***
***   Jason Olejarz
***   2010-07-21
***
***   This is the header file for save.c.
***
***************************************************************
**************************************************************/

struct save_add_data_data
{
	short int data_index_input;
	short int data_index_output;
	int size;
};
struct save_add_data_parameters
{
	int size;
	int index;
};

struct save_brdim_data
{
	short int data_index_step;
	short int data_index_flips;
	short int data_index_brdim;
	int size;
};
struct save_brdim_parameters
{
	int size;
	int index;
};

struct save_clusters_data
{
	short int data_index_number_of_clusters;
	short int data_index_genus;
	int size;
};
struct save_clusters_parameters
{
	int size;
	int index;
};

struct save_constean_data
{
	short int data_index_step;
	short int data_index_previous_steps;
	short int data_index_time_elapsed;
	short int data_index_flips;
	short int data_index_energy;
	short int spin;
	int animation_steps;
	int num;
};
struct save_constean_parameters
{
};

struct save_constebr_data
{
	short int data_index_step;
	short int data_index_previous_steps;
	short int data_index_time_elapsed;
	short int data_index_flips;
	short int data_index_energy;
	short int spin;
	int animation_steps;
	int num;
};
struct save_constebr_parameters
{
};

struct save_lattice_data
{
	int num;
	int prefix;
};
struct save_lattice_parameters
{
};

struct save_perc_data
{
	short int data_index_max_perc;
	short int data_index_perc_x;
	short int data_index_perc_y;
	short int data_index_perc_z;
	int size;
};
struct save_perc_parameters
{
};

struct save_plotfbr_data
{
	int num;
};
struct save_plotfbr_parameters
{
};

struct save_pseudobl_data
{
	short int data_index_volume;
	short int data_index_dim;
	short int data_index_Ix;
	short int data_index_Iy;
	short int data_index_Iz;
	int size;
};
struct save_pseudobl_parameters
{
};

struct save_seed_64_data
{
	short int data_index_low;
	short int data_index_high;
	int size;
};
struct save_seed_64_parameters
{
	int size;
	int index;
};

struct save_value_data
{
	short int data_index;
	int size;
};
struct save_value_parameters
{
	int size;
	int index;
};

short int save_add_data(short int command, struct save_add_data_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_blinker_region_dim(short int command, struct save_brdim_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_check_same_genus(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_conste_animate(short int command, struct save_constean_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_conste_blinker_regions(short int command, struct save_constebr_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_final_active(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_final_blinker_regions(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_final_clusters(short int command, struct save_clusters_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_final_energy(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_final_mag(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_final_time(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_lattice(short int command, struct save_lattice_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_number_of_blinker_regions(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_percolation(short int command, struct save_perc_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_plot_blinker_regions(short int command, struct save_plotfbr_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_pseudo_blinker_stats(short int command, struct save_pseudobl_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_seed(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_seed_64(short int command, struct save_seed_64_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_skip(short int command, void *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int save_steps(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);

