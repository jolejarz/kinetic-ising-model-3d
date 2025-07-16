/**************************************************************
***************************************************************
*** 
***   cmd-step.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains commands for intermediate processing.
***
***************************************************************
**************************************************************/

void cmd_avgtdrop()
{
	short int step_index=arg_short_int[0];
	short int data_index_sum=arg_short_int[1];
	short int data_index_num_times=arg_short_int[2];
	int num=arg_int[0];
	
	struct step_avgtdrop_data avgtdropdata;
	
	avgtdropdata.data_index_sum=data_index_sum;
	avgtdropdata.data_index_num_times=data_index_num_times;
	avgtdropdata.size=num;
	step_avgtdrop(command_insert, &avgtdropdata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_avgtdrop, step_num);
	
	return;
}

void cmd_chk()
{
	short int step_index=arg_short_int[0];
	
	struct step_check_data checkdata;
	
	step_check(command_insert, &checkdata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_check, step_num);
	
	return;
}

void cmd_del()
{
	short int step_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	
	struct step_data_delete_data datadeletedata;
	
	datadeletedata.data_index=data_index;
	step_data_delete(command_insert, &datadeletedata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_data_delete, step_num);
	
	return;
}

void cmd_edrop()
{
	short int step_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int num=arg_int[0];
	
	struct step_edrop_data edropdata;
	
	edropdata.data_index=data_index;
	edropdata.size=num;
	step_edrop(command_insert, &edropdata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_edrop, step_num);
	
	return;
}

void cmd_flips()
{
	short int step_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int steps=arg_int[0];
	
	struct step_value_data flipsdata;
	
	flipsdata.data_index=data_index;
	flipsdata.steps=steps;
	step_flips(command_insert, &flipsdata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_flips, step_num);
	
	return;
}

void cmd_force()
{
	short int step_index=arg_short_int[0];
	double end_time=arg_double[0];
	
	struct step_force_data forcedata;
	
	forcedata.end_time=end_time;

	step_force(command_insert, &forcedata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_force, step_num);

	return;
}

void cmd_forceend()
{
	short int step_index=arg_short_int[0];
	short int num_energy_drops_index=arg_short_int[1];
	short int total_energy_drop_index=arg_short_int[2];
	double end_time=arg_double[0];
	int num=arg_int[0];
	
	struct step_forceend_data forceenddata;
	
	forceenddata.num_energy_drops_index=num_energy_drops_index;
	forceenddata.total_energy_drop_index=total_energy_drop_index;
	forceenddata.end_time=end_time;
	forceenddata.size=num;

	step_force_end(command_insert, &forceenddata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_force_end, step_num);

	return;
}

void cmd_forceenda()
{
	short int step_index=arg_short_int[0];
	short int num_energy_drops_index=arg_short_int[1];
	short int total_energy_drop_index=arg_short_int[2];
	double end_time=arg_double[0];
	int num=arg_int[0];
	
	struct step_forceenda_data forceendadata;
	
	forceendadata.num_energy_drops_index=num_energy_drops_index;
	forceendadata.total_energy_drop_index=total_energy_drop_index;
	forceendadata.end_time=end_time;
	forceendadata.size=num;

	step_force_end_a(command_insert, &forceendadata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_force_end_a, step_num);

	return;
}

void cmd_getcutoff()
{
	short int step_index=arg_short_int[0];
	short int final_energy_data_index=arg_short_int[1];
	int num_spins=arg_int[0];
	double frac_diff=arg_double[0];
	int num=arg_int[1];
	
	struct step_getcutoff_data getcutoffdata;
	
	getcutoffdata.final_energy_data_index=final_energy_data_index;
	getcutoffdata.num_spins=num_spins;
	getcutoffdata.frac_diff=frac_diff;
	getcutoffdata.size=num;

	step_get_cutoff_time(command_insert, &getcutoffdata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_get_cutoff_time, step_num);

	return;
}

void cmd_intfcom()
{
	short int step_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int steps=arg_int[0];
	double initial_com=arg_double[0];
	short int positive=arg_short_int[2];
	
	struct step_intfcom_data intfcomdata;
	
	intfcomdata.data_index=data_index;
	intfcomdata.steps=steps;
	intfcomdata.initial_com=initial_com;
	intfcomdata.positive=positive;
	step_intfcom(command_insert, &intfcomdata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_intfcom, step_num);
	
	return;
}

void cmd_latrng()
{
	short int step_index=arg_short_int[0];
	int num=arg_int[0];
	
	struct step_latrng_data latrngdata;
	
	latrngdata.num=num;
	step_lattice_rng(command_insert, &latrngdata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_lattice_rng, step_num);
	
	return;
}

void cmd_longdrop()
{
	short int step_index=arg_short_int[0];
	short int data_index_step_low=arg_short_int[1];
	short int data_index_step_high=arg_short_int[2];
	int num=arg_int[0];
	
	// 'data_index_step_low' and 'data_index_step_high' contain the steps at which the energy drops
	// after the longest amount of time has elapsed with the lattice at constant energy.
	//
	// 'data_index_step_low': low 32 bits
	// 'data_index_step_high': high 32 bits
	
	struct step_longdrop_data longdropdata;
	
	longdropdata.data_index_step_low=data_index_step_low;
	longdropdata.data_index_step_high=data_index_step_high;
	longdropdata.num=num;
	step_long_drop(command_insert, &longdropdata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_long_drop, step_num);
	
	return;
}

void cmd_magstop()
{
	short int step_index=arg_short_int[0];
	int mag=arg_int[0];
	
	struct step_magstop_data magstopdata;
	
	magstopdata.mag=mag;
	step_mag_stop(command_insert, &magstopdata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_mag_stop, step_num);
	
	return;
}

void cmd_stop()
{
	short int step_index=arg_short_int[0];
	int steps=arg_int[0];
	
	struct step_stop_data stopdata;
	
	stopdata.steps=steps;
	step_stop(command_insert, &stopdata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_stop, step_num);
	
	return;
}

void cmd_stoplong()
{
	short int step_index=arg_short_int[0];
	short int step_data_index_low=arg_short_int[1];
	short int step_data_index_high=arg_short_int[2];
	int num=arg_int[0];
	
	struct step_stop_long_data stoplongdata;
	
	stoplongdata.step_data_index_low=step_data_index_low;
	stoplongdata.step_data_index_high=step_data_index_high;
	stoplongdata.num=num;
	step_stop_long(command_insert, &stoplongdata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_stop_long, step_num);
	
	return;
}

void cmd_svact()
{
	short int step_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int steps=arg_int[0];
	
	struct step_value_data activedata;
	
	activedata.data_index=data_index;
	activedata.steps=steps;
	step_save_active(command_insert, &activedata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_save_active, step_num);
	
	return;
}

void cmd_svconste()
{
	short int step_index=arg_short_int[0];
	short int data_index_step=arg_short_int[1];
	short int data_index_previous_steps=arg_short_int[2];
	short int data_index_time_elapsed=arg_short_int[3];
	int num=arg_int[0];
	
	// 'data_index_step' contains the steps at which the energy drops after the
	// longest amount of time has elapsed with the lattice at constant energy.
	//
	// 'data_index_previous_steps' gives the maximum number of Monte Carlo steps
	// over which the energy was constant.
	//
	// 'data_index_time_elapsed' gives the maximum time over which the energy
	// was constant.
	//
	
	struct step_svconste_data svconstedata;
	
	svconstedata.data_index_step=data_index_step;
	svconstedata.data_index_previous_steps=data_index_previous_steps;
	svconstedata.data_index_time_elapsed=data_index_time_elapsed;
	svconstedata.num=num;
	step_save_conste(command_insert, &svconstedata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_save_conste, step_num);
	
	return;
}

void cmd_sven()
{
	short int step_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int steps=arg_int[0];
	
	struct step_value_data energydata;
	
	energydata.data_index=data_index;
	energydata.steps=steps;
	step_save_energy(command_insert, &energydata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_save_energy, step_num);
	
	return;
}

void cmd_svmag()
{
	short int step_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int steps=arg_int[0];
	
	struct step_value_data magdata;
	
	magdata.data_index=data_index;
	magdata.steps=steps;
	step_save_mag(command_insert, &magdata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_save_mag, step_num);
	
	return;
}

void cmd_svtm()
{
	short int step_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int steps=arg_int[0];
	
	struct step_value_data timedata;
	
	timedata.data_index=data_index;
	timedata.steps=steps;
	step_save_time(command_insert, &timedata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_save_time, step_num);
	
	return;
}

void cmd_svtmdrop()
{
	short int step_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int steps=arg_int[0];
	
	struct step_value_data timedropdata;
	
	timedropdata.data_index=data_index;
	timedropdata.steps=steps;
	step_save_time_drop(command_insert, &timedropdata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_save_time_drop, step_num);
	
	return;
}

void cmd_tdropvst()
{
	short int step_index=arg_short_int[0];
	short int data_index_times=arg_short_int[1];
	short int data_index_drop_times=arg_short_int[2];
	int num=arg_int[0];
	
	struct step_tdropvst_data tdropvstdata;
	
	tdropvstdata.data_index_times=data_index_times;
	tdropvstdata.data_index_drop_times=data_index_drop_times;
	tdropvstdata.size=num;
	step_time_drop_vs_time(command_insert, &tdropvstdata, data, data_parameters, data_num, &parameters);
	
	list_insert(step, step_index, step_time_drop_vs_time, step_num);
	
	return;
}

