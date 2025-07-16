/**************************************************************
***************************************************************
*** 
***   step.h
***
***   Jason Olejarz
***   2010-07-21
***
***   This is the header file for step.c.
***
***************************************************************
**************************************************************/

struct step_avgtdrop_data
{
	short int data_index_sum;
	short int data_index_num_times;
	int size;
};
struct step_avgtdrop_parameters
{
	int size;
	int index;
};

struct step_check_data
{
};

struct step_data_delete_data
{
	short int data_index;
};

struct step_edrop_data
{
	short int data_index;
	int size;
};
struct step_edrop_parameters
{
	int size;
	int index;
};

struct step_force_data
{
	int end_time;
};

struct step_forceend_data
{
	short int num_energy_drops_index;
	short int total_energy_drop_index;
	double end_time;
	int size;
};
struct step_forceend_parameters
{
	int size;
	int index;
};

struct step_forceenda_data
{
	short int num_energy_drops_index;
	short int total_energy_drop_index;
	double end_time;
	int size;
};
struct step_forceenda_parameters
{
	int size;
	int index;
};

struct step_getcutoff_data
{
	short int final_energy_data_index;
	int num_spins;
	double frac_diff;
	int size;
};
struct step_getcutoff_parameters
{
	int size;
	int index;
};

struct step_intfcom_data
{
	short int data_index;
	int steps;
	double initial_com;
	short int positive;
};
struct step_intfcom_parameters
{
	int size;
	int index;
};

struct step_latrng_data
{
	int num;
};

struct step_longdrop_data
{
	short int data_index_step_low;
	short int data_index_step_high;
	int num;
};

struct step_magstop_data
{
	int mag;
};

struct step_stop_data
{
	int steps;
};

struct step_stop_long_data
{
	short int step_data_index_low;
	short int step_data_index_high;
	int num;
};

struct step_svconste_data
{
	short int data_index_step;
	short int data_index_previous_steps;
	short int data_index_time_elapsed;
	int num;
};
struct step_svconste_parameters
{
	int size;
	int index;
};

struct step_tdropvst_data
{
	short int data_index_times;
	short int data_index_drop_times;
	int size;
};
struct step_tdropvst_parameters
{
	int size;
	int index;
};

struct step_value_data
{
	short int data_index;
	int steps;
};
struct step_value_parameters
{
	int size;
	int index;
};

short int step_avgtdrop(short int command, struct step_avgtdrop_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_check(short int command, struct step_check_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_data_delete(short int command, struct step_data_delete_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_edrop(short int command, struct step_edrop_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_flips(short int command, struct step_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_force(short int command, struct step_force_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_force_end(short int command, struct step_forceend_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_force_end_a(short int command, struct step_forceenda_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_get_cutoff_time(short int command, struct step_getcutoff_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_intfcom(short int command, struct step_intfcom_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_lattice_rng(short int command, struct step_latrng_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_mag_stop(short int command, struct step_magstop_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_save_active(short int command, struct step_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_save_energy(short int command, struct step_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_save_mag(short int command, struct step_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_save_time(short int command, struct step_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_save_time_drop(short int command, struct step_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_time_drop_vs_time(short int command, struct step_tdropvst_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);
short int step_stop(short int command, struct step_stop_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters);

