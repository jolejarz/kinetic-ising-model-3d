/**************************************************************
***************************************************************
*** 
***   cmd-save.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains commands for setting up postprocessing.
***
***************************************************************
**************************************************************/

void cmd_adddata()
{
	short int save_index=arg_short_int[0];
	short int data_index_input=arg_short_int[1];
	short int data_index_output=arg_short_int[2];
	int num=arg_int[0];
	
	struct save_add_data_data adddatadata;
	
	adddatadata.data_index_input=data_index_input;
	adddatadata.data_index_output=data_index_output;
	adddatadata.size=num;
	save_add_data(command_insert, &adddatadata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_add_data, save_num);
	
	return;
}

// This function determines the dimensionality (0, 1, 2, or 3)
// of the blinker regions that were responsible for a long-lived
// state.
void cmd_brdim()
{
	short int save_index=arg_short_int[0];
	short int data_index_step=arg_short_int[1];
	short int data_index_flips=arg_short_int[2];
	short int data_index_brdim=arg_short_int[3];
	int size=arg_int[0];
	
	// 'data_index_step' contains the steps at which the energy drops after the
	// longest amount of time has elapsed with the lattice at constant energy.
	//
	// 'num' is the number of simulations.
	//
	
	struct save_brdim_data brdimdata;
	
	brdimdata.data_index_step=data_index_step;
	brdimdata.data_index_flips=data_index_flips;
	brdimdata.data_index_brdim=data_index_brdim;
	brdimdata.size=size;
	save_blinker_region_dim(command_insert, &brdimdata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_blinker_region_dim, save_num);

	return;
}

void cmd_chksameg()
{
	short int save_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int size=arg_int[0];
	
	struct save_value_data chksamegdata;
	
	chksamegdata.data_index=data_index;
	chksamegdata.size=size;
	save_check_same_genus(command_insert, &chksamegdata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_check_same_genus, save_num);

	return;
}

// This function saves an animation of each simulation just before
// the energy drops to end a long-lived metastable state.
void cmd_constean()
{
	short int save_index=arg_short_int[0];
	short int data_index_step=arg_short_int[1];
	short int data_index_previous_steps=arg_short_int[2];
	short int data_index_time_elapsed=arg_short_int[3];
	short int data_index_flips=arg_short_int[4];
	short int data_index_energy=arg_short_int[5];
	short int spin=arg_short_int[6];
	int animation_steps=arg_int[0];
	int num=arg_int[1];
	
	// 'data_index_step' contains the steps at which the energy drops after the
	// longest amount of time has elapsed with the lattice at constant energy.
	//
	// 'data_index_previous_steps' gives the maximum number of Monte Carlo steps
	// over which the energy was constant.
	//
	// 'data_index_time_elapsed' gives the maximum time over which the energy
	// was constant.
	//
	// 'animation_steps' is the number of steps before the drop in energy
	// that are to be included in the animation.
	//
	// 'num' is the number of simulations to save an animation for.
	//
	
	// All files are saved as 'd1', 'd2', ... , 'dN' and 'p1.m', 'p2.m', ... ,
	// 'pN.m', where N is equal to 'num'.
	//
	
	struct save_constean_data consteandata;
	
	consteandata.data_index_step=data_index_step;
	consteandata.data_index_previous_steps=data_index_previous_steps;
	consteandata.data_index_time_elapsed=data_index_time_elapsed;
	consteandata.data_index_flips=data_index_flips;
	consteandata.data_index_energy=data_index_energy;
	consteandata.spin=spin;
	consteandata.animation_steps=animation_steps;
	consteandata.num=num;
	save_conste_animate(command_insert, &consteandata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_conste_animate, save_num);

	return;
}

// This function saves a plot of the blinker regions
// that were responsible for the longest constant-energy
// metastable state.
void cmd_constebr()
{
	short int save_index=arg_short_int[0];
	short int data_index_step=arg_short_int[1];
	short int data_index_previous_steps=arg_short_int[2];
	short int data_index_time_elapsed=arg_short_int[3];
	short int data_index_flips=arg_short_int[4];
	short int data_index_energy=arg_short_int[5];
	short int spin=arg_short_int[6];
	int animation_steps=arg_int[0];
	int num=arg_int[1];
	
	// 'data_index_step' contains the steps at which the energy drops after the
	// longest amount of time has elapsed with the lattice at constant energy.
	//
	// 'data_index_previous_steps' gives the maximum number of Monte Carlo steps
	// over which the energy was constant.
	//
	// 'data_index_time_elapsed' gives the maximum time over which the energy
	// was constant.
	//
	// 'animation_steps' is the number of steps before the drop in energy
	// that are to be included in the animation.
	//
	// 'num' is the number of simulations to save a plot for.
	//
	
	// All files are saved as 'd1', 'd2', ... , 'dN' and 'p1.m', 'p2.m', ... ,
	// 'pN.m', where N is equal to 'num'.
	//
	
	struct save_constebr_data constebrdata;
	
	constebrdata.data_index_step=data_index_step;
	constebrdata.data_index_previous_steps=data_index_previous_steps;
	constebrdata.data_index_time_elapsed=data_index_time_elapsed;
	constebrdata.data_index_flips=data_index_flips;
	constebrdata.data_index_energy=data_index_energy;
	constebrdata.spin=spin;
	constebrdata.animation_steps=animation_steps;
	constebrdata.num=num;
	save_conste_blinker_regions(command_insert, &constebrdata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_conste_blinker_regions, save_num);

	return;
}

void cmd_fa()
{
	short int save_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int num=arg_int[0];
	
	struct save_value_data valuedata;
	
	valuedata.data_index=data_index;
	valuedata.size=num;
	save_final_active(command_insert, &valuedata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_final_active, save_num);
	
	return;
}

void cmd_fbr()
{
	short int save_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int num=arg_int[0];
	
	struct save_value_data blinkerregionsdata;
	
	blinkerregionsdata.data_index=data_index;
	blinkerregionsdata.size=num;
	save_final_blinker_regions(command_insert, &blinkerregionsdata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_final_blinker_regions, save_num);
	
	return;
}

void cmd_fcl()
{
	short int save_index=arg_short_int[0];
	short int data_index_number_of_clusters=arg_short_int[1];
	short int data_index_genus=arg_short_int[2];
	int num=arg_int[0];
	
	struct save_clusters_data clustersdata;
	
	clustersdata.data_index_number_of_clusters=data_index_number_of_clusters;
	clustersdata.data_index_genus=data_index_genus;
	clustersdata.size=num;
	save_final_clusters(command_insert, &clustersdata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_final_clusters, save_num);
	
	return;
}

void cmd_fe()
{
	short int save_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int num=arg_int[0];
	
	struct save_value_data valuedata;
	
	valuedata.data_index=data_index;
	valuedata.size=num;
	save_final_energy(command_insert, &valuedata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_final_energy, save_num);
	
	return;
}

void cmd_fm()
{
	short int save_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int num=arg_int[0];
	
	struct save_value_data valuedata;
	
	valuedata.data_index=data_index;
	valuedata.size=num;
	save_final_mag(command_insert, &valuedata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_final_mag, save_num);
	
	return;
}

void cmd_fs()
{
	short int save_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int num=arg_int[0];
	
	struct save_value_data valuedata;
	
	valuedata.data_index=data_index;
	valuedata.size=num;
	save_steps(command_insert, &valuedata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_steps, save_num);
	
	return;
}

void cmd_ft()
{
	short int save_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int num=arg_int[0];
	
	struct save_value_data valuedata;
	
	valuedata.data_index=data_index;
	valuedata.size=num;
	save_final_time(command_insert, &valuedata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_final_time, save_num);
	
	return;
}

void cmd_numbr()
{
	short int save_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int num=arg_int[0];
	
	struct save_value_data valuedata;
	
	valuedata.data_index=data_index;
	valuedata.size=num;
	save_number_of_blinker_regions(command_insert, &valuedata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_number_of_blinker_regions, save_num);
	
	return;
}

void cmd_perc()
{
	short int save_index=arg_short_int[0];
	short int data_index_max_perc=arg_short_int[1];
	short int data_index_perc_x=arg_short_int[2];
	short int data_index_perc_y=arg_short_int[3];
	short int data_index_perc_z=arg_short_int[4];
	int num=arg_int[0];
	
	struct save_perc_data percdata;
	
	percdata.data_index_max_perc=data_index_max_perc;
	percdata.data_index_perc_x=data_index_perc_x;
	percdata.data_index_perc_y=data_index_perc_y;
	percdata.data_index_perc_z=data_index_perc_z;
	percdata.size=num;
	save_percolation(command_insert, &percdata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_percolation, save_num);
	
	return;
}

// This function saves a plot of the blinker regions
// at the end of each run.
void cmd_plotfbr()
{
	short int save_index=arg_short_int[0];
	int num=arg_int[0];
	
	// 'num' is the number of simulations to save a plot for.
	//
	
	// All files are saved as 'd1', 'd2', ... , 'dN' and 'p1.m', 'p2.m', ... ,
	// 'pN.m', where N is equal to 'num'.
	//
	
	struct save_plotfbr_data plotfbrdata;
	
	plotfbrdata.num=num;
	save_plot_blinker_regions(command_insert, &plotfbrdata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_plot_blinker_regions, save_num);

	return;
}

void cmd_pseudobl()
{
	short int save_index=arg_short_int[0];
	short int volume_data_index=arg_short_int[1];
	short int dim_data_index=arg_short_int[2];
	short int Ix_data_index=arg_short_int[3];
	short int Iy_data_index=arg_short_int[4];
	short int Iz_data_index=arg_short_int[5];
	int size=arg_int[0];
	
	struct save_pseudobl_data pseudobldata;
	
	pseudobldata.data_index_volume=volume_data_index;
	pseudobldata.data_index_dim=dim_data_index;
	pseudobldata.data_index_Ix=Ix_data_index;
	pseudobldata.data_index_Iy=Iy_data_index;
	pseudobldata.data_index_Iz=Iz_data_index;
	pseudobldata.size=size;
	save_pseudo_blinker_stats(command_insert, &pseudobldata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_pseudo_blinker_stats, save_num);
	
	return;
}

void cmd_svlat()
{
	short int save_index=arg_short_int[0];
	int num=arg_int[0];
	int prefix=arg_int[1];
	
	struct save_lattice_data svlatdata;
	
	svlatdata.num=num;
	svlatdata.prefix=prefix;
	save_lattice(command_insert, &svlatdata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_lattice, save_num);
	
	return;
}

void cmd_svseed()
{
	short int save_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int num=arg_int[0];
	
	struct save_value_data valuedata;
	
	valuedata.data_index=data_index;
	valuedata.size=num;
	save_seed(command_insert, &valuedata, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_seed, save_num);
	
	return;
}

void cmd_svseed64()
{
	short int save_index=arg_short_int[0];
	short int data_index_low=arg_short_int[1];
	short int data_index_high=arg_short_int[2];
	int num=arg_int[0];
	
	struct save_seed_64_data seed64data;
	
	seed64data.data_index_low=data_index_low;
	seed64data.data_index_high=data_index_high;
	seed64data.size=num;
	save_seed_64(command_insert, &seed64data, data, data_parameters, data_num, &parameters);
	
	list_insert(save, save_index, save_seed_64, save_num);
	
	return;
}

