/**************************************************************
***************************************************************
*** 
***   cmd-plot.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains graphics commands.
***
***************************************************************
**************************************************************/

void cmd_active()
{
	short int data_index=arg_short_int[0];
	int plot_steps=arg_int[0];
	short int spin=arg_short_int[1];
	
	short int *spins=malloc(sizeof(short int)*parameters.nx*parameters.ny*parameters.nz);
	
	memcpy(spins, parameters.spins_initial, sizeof(short int)*parameters.nx*parameters.ny*parameters.nz);

	int *flips, *flips_size;

	if (plot_steps>0)
	{
		if (data_input(data, data_parameters, &flips, &flips_size, data_index, data_num)==1) return;
		state(spins, flips, plot_steps);
	}
	
	FILE *data, *plot;
	
	data=fopen(arg_string[0], "w");
	plot=fopen(arg_string[1], "w");
	active_plot(spins, parameters.nx, parameters.ny, parameters.nz, parameters.periodic_bc_x, parameters.periodic_bc_y, parameters.periodic_bc_z, plot_steps, spin, arg_string[0], data, plot);
	
	fclose(plot);
	fclose(data);
	
	free(spins);
	
	return;
}

void cmd_animate()
{
	short int data_index_flips=arg_short_int[0];
	short int data_index_energy=arg_short_int[1];
	short int plottype=arg_short_int[2];
	short int spin=arg_short_int[3];
	int steps_begin=arg_int[0];
	int steps=arg_int[1];
	int interval=arg_int[2];
	double delay=arg_double[0];

	short int *spins=malloc(sizeof(short int)*parameters.nx*parameters.ny*parameters.nz);
	
	memcpy(spins, parameters.spins_initial, sizeof(short int)*parameters.nx*parameters.ny*parameters.nz);
	
	int *flips, *flips_size;

	if (data_input(data, data_parameters, &flips, &flips_size, data_index_flips, data_num)==1) return;

	double *energy;
	int *energy_size;
	
	if (data_input(data, data_parameters, &energy, &energy_size, data_index_energy, data_num)==1) return;

	state(spins, flips, steps_begin);

	FILE *data, *plot;

	if (plottype==0)
	{
		data=fopen(arg_string[0], "w");
		plot=fopen(arg_string[1], "w");
		animate(spins, flips+2*steps_begin, energy+steps_begin, steps_begin, steps-steps_begin, interval, parameters.nx, parameters.ny, parameters.nz, spin, delay, arg_string[0], data, plot);
	}
	else
	{
		data=fopen(arg_string[0], "w");
		plot=fopen(arg_string[1], "w");
		animate_step(spins, flips+2*steps_begin, energy+steps_begin, steps_begin, steps-steps_begin, parameters.nx, parameters.ny, parameters.nz, spin, arg_string[0], data, plot);
	}
	
	fclose(plot);
	fclose(data);
	
	free(spins);
	
	return;
}

void cmd_animatert()
{
	short int data_index_flips=arg_short_int[0];
	short int data_index_energy=arg_short_int[1];
	short int data_index_time=arg_short_int[2];
	short int plottype=arg_short_int[3];
	short int spin=arg_short_int[4];
	double time_begin=arg_double[0];
	double time_end=arg_double[1];
	double time_between_frames=arg_double[2];
	double delay=arg_double[3];

	short int *spins=malloc(sizeof(short int)*parameters.nx*parameters.ny*parameters.nz);
	
	memcpy(spins, parameters.spins_initial, sizeof(short int)*parameters.nx*parameters.ny*parameters.nz);
	
	int *flips, *flips_size;

	if (data_input(data, data_parameters, &flips, &flips_size, data_index_flips, data_num)==1) return;

	double *energy;
	int *energy_size;
	
	if (data_input(data, data_parameters, &energy, &energy_size, data_index_energy, data_num)==1) return;

	double *time;
	int *time_size;
	
	if (data_input(data, data_parameters, &time, &time_size, data_index_time, data_num)==1) return;

	// Determine the step that corresponds with 'time_begin'.
	int steps_begin=0;
	while (1)
	{
		if (*(time+steps_begin)>=time_begin) break;
		steps_begin++;
	}

	// Determine the step that corresponds with 'time_end'.
	// If 'time_end' is longer than the simulation lasted, then
	// 'steps_end' is set to the maximum number of steps.
	int steps_end=steps_begin;
	while (1)
	{
		if (*(time+steps_end)>=time_end || steps_end==*time_size/sizeof(double)-1) break;
		steps_end++;
	}

	state(spins, flips, steps_begin);

	// Save a plot of the spins at 'steps_begin'.
	arg_short_int[0]=data_index_flips;
	arg_short_int[1]=1;
	arg_int[0]=steps_begin;
	arg_short_int[2]=spin;
	cmd_spins();
	
	FILE *data, *plot;

	if (plottype==0)
	{
		data=fopen(arg_string[2], "w");
		plot=fopen(arg_string[3], "w");
		animate_real_time(spins, flips+2*steps_begin, energy+steps_begin, time+steps_begin, steps_begin, steps_end-steps_begin, time_between_frames, parameters.nx, parameters.ny, parameters.nz, spin, delay, arg_string[2], data, plot);
	}
	else
	{
		data=fopen(arg_string[2], "w");
		plot=fopen(arg_string[3], "w");
		animate_real_time_step(spins, flips+2*steps_begin, energy+steps_begin, time+steps_begin, steps_begin, steps_end-steps_begin, parameters.nx, parameters.ny, parameters.nz, spin, arg_string[2], data, plot);
	}
	
	fclose(plot);
	fclose(data);
	
	free(spins);
	
	return;
}

void cmd_bregplot()
{
	short int data_index=arg_short_int[0];

	short int *regions;
	int *regions_size;

	if (data_input(data, data_parameters, &regions, &regions_size, data_index, data_num)==1) return;

	FILE *data=fopen(arg_string[0], "w");
	FILE *plot=fopen(arg_string[1], "w");
	
	blinkers_plot(regions, parameters.nx, parameters.ny, parameters.nz, arg_string[0], data, plot);
	
	fclose(data);
	fclose(plot);
	
	return;
}

void cmd_bregplotm()
{
	short int data_index=arg_short_int[0];

	short int *regions;
	int *regions_size;

	if (data_input(data, data_parameters, &regions, &regions_size, data_index, data_num)==1) return;

	FILE *data=fopen(arg_string[0], "w");
	FILE *plot=fopen(arg_string[1], "w");
	
	blinkers_plot_matlab(regions, parameters.nx, parameters.ny, parameters.nz, arg_string[0], data, plot);
	
	fclose(data);
	fclose(plot);
	
	return;
}

void cmd_clspins()
{
	int cluster_number=arg_int[0];
	
	int nxtimesnytimesnz=parameters.nx*parameters.ny*parameters.nz;
	
	int *lattice=(int*)malloc (sizeof(int)*nxtimesnytimesnz);
	struct cluster_stats *cluster_list=(struct cluster_stats*)malloc (sizeof(struct cluster_stats)*nxtimesnytimesnz);
	short int *cluster_spins=(short int*)malloc (sizeof(short int)*nxtimesnytimesnz);

	// Label the clusters.
	clusters (&parameters, lattice, parameters.spins);
	
	// Get cluster statistics.
	cluster_stats (&parameters, cluster_list, lattice, parameters.spins);
	
	// Get the cluster's spins.
	save_cluster_spins (&parameters, cluster_spins, cluster_number, cluster_list, lattice);
	
	FILE *data=fopen (arg_string[0], "w");
	FILE *plot=fopen (arg_string[1], "w");
	
	cluster_spins_plot (cluster_spins, parameters.nx, parameters.ny, parameters.nz, arg_string[0], data, plot);

	fclose (plot);
	fclose (data);
	
	free (cluster_spins);
	
	return;
}

void cmd_spins()
{
	short int data_index=arg_short_int[0];
	short int graphics=arg_short_int[1];
	int plot_steps=arg_int[0];
	short int spin=arg_short_int[2];
	
	short int *spins=malloc(sizeof(short int)*parameters.nx*parameters.ny*parameters.nz);
	
	memcpy(spins, parameters.spins_initial, sizeof(short int)*parameters.nx*parameters.ny*parameters.nz);

	int *flips, *flips_size;

	if (plot_steps>0)
	{
		if (data_input(data, data_parameters, &flips, &flips_size, data_index, data_num)==1) return;
		state(spins, flips, plot_steps);
	}
	
	FILE *data, *plot;
	
	if (graphics==0)
	{
		data=fopen(arg_string[0], "w");
		plot=fopen(arg_string[1], "w");
		state_plot_gnuplot(spins, parameters.nx, parameters.ny, parameters.nz, plot_steps, spin, arg_string[0], data, plot);
	}
	else
	{
		data=fopen(arg_string[0], "w");
		plot=fopen(arg_string[1], "w");
		state_plot_matlab(spins, parameters.nx, parameters.ny, parameters.nz, plot_steps, spin, arg_string[0], data, plot);
	}
	
	fclose(plot);
	fclose(data);
	
	free(spins);
	
	return;
}

void cmd_xy()
{
	short int data_index_x=arg_short_int[0];
	short int data_index_y=arg_short_int[1];

	if (data[data_index_x]==0)
	{
		printf("The x array is empty.\n");
		return;
	}
	if (data[data_index_y]==0)
	{
		printf("The y array is empty.\n");
		return;
	}

	double *x_data=data[data_index_x], *y_data=data[data_index_y];
	
	int *x_size=data_parameters[data_index_x], *y_size=data_parameters[data_index_y];
	if (*x_size!=*y_size)
	{
		printf("The sizes of the x and y arrays must be the same.\n");
		return;
	}
	
	FILE *data=fopen(arg_string[0], "w");
	FILE *plot=fopen(arg_string[1], "w");

	xy_plot(x_data, y_data, *x_size/sizeof(double), arg_string[0], data, plot);
	
	fclose(plot);
	fclose(data);
	
	return;
}

