/**************************************************************
***************************************************************
*** 
***   save.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains the routines used by 'calc' to save data after each run of 'evolve'.
***
***************************************************************
**************************************************************/

#include "includes.h"

short int save_add_data(short int command, struct save_add_data_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static short int data_index_input;
	static short int data_index_output;
	
	static double *input_data;
	static int *input_data_size;
	
	static double *output_data;
	
	static struct save_add_data_data b;
	static struct save_add_data_parameters *a;
	
	switch (command)
	{
		case command_info: {printf("save_add_data\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_add_data_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index_input=b.data_index_input;
			data_index_output=b.data_index_output;

			if (data_output(data, data_parameters, &output_data, &a, b.size*sizeof(double), sizeof(struct save_add_data_parameters), data_index_output, data_size)==1) return result_error;

			a->index=0;
			a->size=0;
			
			return result_continue;
		}
		case command_run:
		{
			if (data_input(data, data_parameters, &input_data, &input_data_size, data_index_input, data_size)==1) return result_error;

			memcpy(output_data+(a->index), input_data, *input_data_size);
			(a->size)+=*input_data_size;
			(a->index)+=*input_data_size/sizeof(double);
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_blinker_region_dim(short int command, struct save_brdim_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int nx;
	static int nxtimesny;
	static int nxtimesnytimesnz;
	
	static short int data_index_step;
	static short int data_index_flips;
	static short int data_index_brdim;
	static int size;
	
	static double *step_array;
	static int *step_array_size;
	
	static int i, j;
	
	static int steps_begin;
	
	static short int *spins;
	static int *flips;
	static int *flips_size;
	
	static short int *regions_before_energy_drop, *regions_after_energy_drop, *regions;
	
	static double *brdim;
	static int *brdim_size;
	
	// These variables are initialized to '1' before checking the blinker region dimensionality.
	// If two blinker spins are found to have different x, y, or z coordinates, then 'same_x',
	// 'same_y', or 'same_z' is set to '0', respectively.
	static short int same_x, same_y, same_z;
	
	static short int x_ref, y_ref, z_ref;
	static short int x, y, z;
	
	static short int dimensions;
	
	static struct save_brdim_data b;
	static struct save_brdim_parameters *a;
	
	switch (command)
	{
		case command_info: {printf("save_blinker_region_dim\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_brdim_data));
			
			nx=parameters->nx;
			nxtimesny=nx*(parameters->ny);
			nxtimesnytimesnz=nxtimesny*(parameters->nz);

			spins=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			regions_before_energy_drop=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			regions_after_energy_drop=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			regions=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);

			return result_continue;
		}
		case command_begin:
		{
			if (parameters->nx!=nx || nx*(parameters->ny)!=nxtimesny || nxtimesny*(parameters->nz)!=nxtimesnytimesnz)
			{
				nx=parameters->nx;
				nxtimesny=nx*(parameters->ny);
				nxtimesnytimesnz=nxtimesny*(parameters->nz);
				
				spins=(short int*)realloc (spins, sizeof(short int)*nxtimesnytimesnz);
				regions_before_energy_drop=(short int*)realloc (regions_before_energy_drop, sizeof(short int)*nxtimesnytimesnz);
				regions_after_energy_drop=(short int*)realloc (regions_after_energy_drop, sizeof(short int)*nxtimesnytimesnz);
				regions=(short int*)realloc (regions, sizeof(short int)*nxtimesnytimesnz);
			}
			
			data_index_step=b.data_index_step;
			data_index_flips=b.data_index_flips;
			data_index_brdim=b.data_index_brdim;
			size=b.size;

			if (data_input(data, data_parameters, &step_array, &step_array_size, data_index_step, data_size)==1) return result_error;

			if (data_output(data, data_parameters, &brdim, &a, b.size*sizeof(double), sizeof(struct save_brdim_parameters), data_index_brdim, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;

			return result_continue;
		}
		case command_run:
		{
			if (data_input(data, data_parameters, &flips, &flips_size, data_index_flips, data_size)==1) return result_error;

			// Determine the step before the energy dropped.
			steps_begin=*(step_array+(a->index))-1;

			// Load the initial lattice from the previous simulation.
			memcpy(spins, parameters->spins_initial, sizeof(short int)*nxtimesnytimesnz);

			// Evolve the initial lattice to the step before the energy dropped.
			state(spins, flips, steps_begin);

			// Save the blinker regions before the energy drop.
			blinker_regions (parameters, data, data_parameters, data_size, data_index_flips, spins, regions_before_energy_drop, 0);
			
			// Evolve the initial lattice one more step so that the energy drops.
			state(spins, flips+2*steps_begin, 1);

			// Save the blinker regions after the energy drop.
			blinker_regions (parameters, data, data_parameters, data_size, data_index_flips, spins, regions_after_energy_drop, 0);
			
			// Determine the regions that were responsible for the long-lived
			// metastable state.
			for (j=0; j<nxtimesnytimesnz; j++)
			{
				*(regions+j)=*(regions_before_energy_drop+j)-*(regions_after_energy_drop+j);
			}

			// Get the dimensionality of the blinker regions that were responsible for
			// the long-lived state.
			
			dimensions=0;
			same_x=1; same_y=1; same_z=1;
			
			for (i=0; i<nxtimesnytimesnz; i++)
			{
				// If the selected spin is a blinker spin, then exit this loop.
				// This spin will serve as a reference spin.
				// The coordinates of all other spins with be compared with this spin.
				if (*(regions+i)==1)
				{
					z_ref=(int)(i/nxtimesny);
					y_ref=(int)((i-z_ref*nxtimesny)/nx);
					x_ref=(int)(i-z_ref*nxtimesny-y_ref*nx);
					break;
				}
			}
			
			for (j=i+1; j<nxtimesnytimesnz; j++)
			{
				if (*(regions+j)==1)
				{
					z=(int)(j/nxtimesny);
					y=(int)((j-z*nxtimesny)/nx);
					x=(int)(j-z*nxtimesny-y*nx);
					
					if (same_x==1 && x!=x_ref) {same_x=0; dimensions++;}
					if (same_y==1 && y!=y_ref) {same_y=0; dimensions++;}
					if (same_z==1 && z!=z_ref) {same_z=0; dimensions++;}
				}
			}

			*(brdim+(a->index)++)=(double)dimensions;
			a->size+=sizeof(double);

			flips=0; flips_size=0;
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_conste_animate(short int command, struct save_constean_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int nxtimesnytimesnz;
	
	static short int data_index_step;
	static short int data_index_previous_steps;
	static short int data_index_time_elapsed;
	static short int data_index_flips;
	static short int data_index_energy;
	static short int spin;
	static int animation_steps;
	static int num;
	
	static double *step_array;
	static int *step_array_size;
	
	static int i;
	
	static int steps_begin;
	
	static short int *spins;
	static int *flips;
	static int *flips_size;
	static double *energy;
	static int *energy_size;
	
	static FILE *data_file, *plot_file;
	
	static char data_file_name[6]={}, plot_file_name[6]={};
	
	static struct save_constean_data b;
	static struct save_constean_parameters *a;
	
	switch (command)
	{
		case command_info: {printf("save_conste_animate\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_constean_data));
			
			nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);

			spins=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);

			return result_continue;
		}
		case command_begin:
		{
			if ((parameters->nx)*(parameters->ny)*(parameters->nz)!=nxtimesnytimesnz)
			{
				nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);
				spins=(short int*)realloc (spins, sizeof(short int)*nxtimesnytimesnz);
			}
			
			data_index_step=b.data_index_step;
			data_index_previous_steps=b.data_index_previous_steps;
			data_index_time_elapsed=b.data_index_time_elapsed;
			data_index_flips=b.data_index_flips;
			data_index_energy=b.data_index_energy;
			spin=b.spin;
			animation_steps=b.animation_steps;
			num=b.num;

			if (data_input(data, data_parameters, &step_array, &step_array_size, data_index_step, data_size)==1) return result_error;

			// Set 'i' to zero. 'i' is incremented after each run.
			// When i==num, no more animations are saved.
			i=0;
			
			return result_continue;
		}
		case command_run:
		{
			if (data_input(data, data_parameters, &flips, &flips_size, data_index_flips, data_size)==1) return result_error;
			if (data_input(data, data_parameters, &energy, &energy_size, data_index_energy, data_size)==1) return result_error;

			// Check if this realization is to be animated.
			if (i<num)
			{
				// Determine the starting step for the animation.
				steps_begin=*(step_array+i)-animation_steps;

				// Load the initial lattice from the previous simulation.
				memcpy(spins, parameters->spins_initial, sizeof(short int)*nxtimesnytimesnz);

				// Evolve the initial lattice to the starting step for the animation.
				state(spins, flips, steps_begin);

				// Get the filenames for the data and plot files.
				sprintf (data_file_name, "d%d", i);
				sprintf (plot_file_name, "p%d.m", i);

				// Open the data and plot files.
				data_file=fopen(data_file_name, "w"); plot_file=fopen(plot_file_name, "w");

				// Animate the drop in energy.
				animate_step(spins, flips+2*steps_begin, energy+steps_begin, steps_begin, animation_steps, parameters->nx, parameters->ny, parameters->nz, spin, data_file_name, data_file, plot_file);

				// Close the data and plot files.
				fclose(data_file); fclose(plot_file);

				// Increment 'i'.
				i++;
			}

			flips=0; flips_size=0;
			energy=0; energy_size=0;
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_conste_blinker_regions(short int command, struct save_constebr_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int nxtimesnytimesnz;
	
	static short int data_index_step;
	static short int data_index_previous_steps;
	static short int data_index_time_elapsed;
	static short int data_index_flips;
	static short int data_index_energy;
	static short int spin;
	static int animation_steps;
	static int num;
	
	static double *step_array;
	static int *step_array_size;
	
	static double *previous_steps_array;
	static int *previous_steps_array_size;
	
	static int i, j;
	
	static int steps_begin;
	
	static short int *spins;
	static int *flips;
	static int *flips_size;
	static double *energy;
	static int *energy_size;
	
	static short int *regions_before_energy_drop, *regions_after_energy_drop, *regions;
	
	static FILE *data_file, *plot_file;
	
	static char data_file_name[6]={}, plot_file_name[6]={};
	
	static struct save_constebr_data b;
	static struct save_constebr_parameters *a;
	
	switch (command)
	{
		case command_info: {printf("save_conste_blinker_regions\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_constebr_data));
			
			nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);

			spins=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			regions_before_energy_drop=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			regions_after_energy_drop=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			regions=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);

			return result_continue;
		}
		case command_begin:
		{
			if ((parameters->nx)*(parameters->ny)*(parameters->nz)!=nxtimesnytimesnz)
			{
				nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);
				
				spins=(short int*)realloc (spins, sizeof(short int)*nxtimesnytimesnz);
				regions_before_energy_drop=(short int*)realloc (regions_before_energy_drop, sizeof(short int)*nxtimesnytimesnz);
				regions_after_energy_drop=(short int*)realloc (regions_after_energy_drop, sizeof(short int)*nxtimesnytimesnz);
				regions=(short int*)realloc (regions, sizeof(short int)*nxtimesnytimesnz);
			}
			
			data_index_step=b.data_index_step;
			data_index_previous_steps=b.data_index_previous_steps;
			data_index_time_elapsed=b.data_index_time_elapsed;
			data_index_flips=b.data_index_flips;
			data_index_energy=b.data_index_energy;
			spin=b.spin;
			animation_steps=b.animation_steps;
			num=b.num;

			if (data_input(data, data_parameters, &step_array, &step_array_size, data_index_step, data_size)==1) return result_error;
			if (data_input(data, data_parameters, &previous_steps_array, &previous_steps_array_size, data_index_step, data_size)==1) return result_error;

			// Set 'i' to zero. 'i' is incremented after each run.
			// When i==num, no more animations are saved.
			i=0;
			
			return result_continue;
		}
		case command_run:
		{
			if (data_input(data, data_parameters, &flips, &flips_size, data_index_flips, data_size)==1) return result_error;
			if (data_input(data, data_parameters, &energy, &energy_size, data_index_energy, data_size)==1) return result_error;

			// Check if this realization is to be plotted.
			if (i<num)
			{
				// Determine the step before the energy dropped.
				steps_begin=*(step_array+i)-1;

				// Load the initial lattice from the previous simulation.
				memcpy(spins, parameters->spins_initial, sizeof(short int)*nxtimesnytimesnz);

				// Evolve the initial lattice to the step before the energy dropped.
				state(spins, flips, steps_begin);

				// Save the blinker regions before the energy drop.
				blinker_regions (parameters, data, data_parameters, data_size, data_index_flips, spins, regions_before_energy_drop, 0);
				
				// Evolve the initial lattice one more step so that the energy drops.
				state(spins, flips+2*steps_begin, 1);

				// Save the blinker regions after the energy drop.
				blinker_regions (parameters, data, data_parameters, data_size, data_index_flips, spins, regions_after_energy_drop, 0);
				
				// Determine the regions that were responsible for the long-lived
				// metastable state.
				for (j=0; j<nxtimesnytimesnz; j++)
				{
					*(regions+j)=*(regions_before_energy_drop+j)-*(regions_after_energy_drop+j);
				}
				
				// Get the filenames for the data and plot files.
				sprintf (data_file_name, "d%d", i);
				sprintf (plot_file_name, "p%d", i);

				// Open the data and plot files.
				data_file=fopen(data_file_name, "w"); plot_file=fopen(plot_file_name, "w");

				// Plot the blinker regions.
				blinkers_plot (regions, parameters->nx, parameters->ny, parameters->nz, data_file_name, data_file, plot_file);

				// Close the data and plot files.
				fclose(data_file); fclose(plot_file);

				// Increment 'i'.
				i++;
			}

			flips=0; flips_size=0;
			energy=0; energy_size=0;
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_final_active(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index;
	
	static double *active;
	
	static struct save_value_data b;
	static struct save_value_parameters *a;
	
	switch (command)
	{
		case command_info: {printf("save_final_active\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_value_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index=b.data_index;
			
			if (data_output(data, data_parameters, &active, &a, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;
			
			return result_continue;
		}
		case command_run:
		{
			*(active+(a->index)++)=(double)parameters->active;
			a->size+=sizeof(double);
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_final_blinker_regions(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static short int data_index;
	
	static short int *temp;
	
	static double *regions;
	
	static int i, count, nxtimesnytimesnz;
	
	static struct save_value_data b;
	static struct save_value_parameters *a;
	
	switch (command)
	{
		case command_info: {printf("save_final_blinker_regions\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_value_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index=b.data_index;
			
			if (data_output(data, data_parameters, &regions, &a, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;
			
			if ((temp=(short int*)malloc(sizeof(short int)*(parameters->nx)*(parameters->ny)*(parameters->nz)))==0)
			{
				printf("Insufficient memory.\n");
				return result_error;
			}
			
			return result_continue;
		}
		case command_end:
		{
			if (temp!=0) free(temp);
			return result_continue;
		}
		case command_run:
		{
			nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);
			
			blinker_regions(parameters, data, data_parameters, data_size, 0, parameters->spins, temp, 0);
			
			count=0;
			
			for (i=0; i<nxtimesnytimesnz; i++)
			{
				if (*(temp+i)==1) count++;
			}
			
			*(regions+(a->index)++)=(double)count;
			a->size+=sizeof(double);
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_final_clusters(short int command, struct save_clusters_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static short int data_index_number_of_clusters;
	static short int data_index_genus;
	
	static double *cluster_count;
	static double *genus_array;
	
	static int i, count, nxtimesnytimesnz;

	static int number_of_clusters, genus;
	
	static int lowest_euler_characteristic;
	
	static int *lattice;
	static struct cluster_stats *cluster_list;
	static short int *cluster_spins;
	static struct cluster_topology cluster_topology;
	
	static struct save_clusters_data b;
	static struct save_clusters_parameters *a_clusters;
	static struct save_clusters_parameters *a_genus;
	
	switch (command)
	{
		case command_info: {printf("save_final_clusters\n"); return result_continue;}
		case command_insert:
		{
			// Boundary conditions must be either free in all directions or
			// periodic in all directions.
			if (	!(parameters->periodic_bc_x==0 && parameters->periodic_bc_y==0 && parameters->periodic_bc_z==0) &&
				!(parameters->periodic_bc_x==1 && parameters->periodic_bc_y==1 && parameters->periodic_bc_z==1))
			{
				printf("Boundary conditions must be either free in all directions or periodic in all directions.\n");
				return result_error;
			}

			memcpy(&b, command_data, sizeof(struct save_clusters_data));
			
			nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);

			lattice=(int*)malloc (sizeof(int)*nxtimesnytimesnz);
			cluster_list=(struct cluster_stats*)malloc (sizeof(struct cluster_stats)*nxtimesnytimesnz);
			cluster_spins=(short int*)malloc (sizeof(short int)*nxtimesnytimesnz);
			
			return result_continue;
		}
		case command_begin:
		{
			if ((parameters->nx)*(parameters->ny)*(parameters->nz)!=nxtimesnytimesnz)
			{
				nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);

				lattice=(int*)realloc (lattice, sizeof(int)*nxtimesnytimesnz);
				cluster_list=(struct cluster_stats*)realloc (cluster_list, sizeof(struct cluster_stats)*nxtimesnytimesnz);
				cluster_spins=(short int*)realloc (cluster_spins, sizeof(short int)*nxtimesnytimesnz);
			}
			
			data_index_number_of_clusters=b.data_index_number_of_clusters;
			data_index_genus=b.data_index_genus;
			
			if (data_output(data, data_parameters, &cluster_count, &a_clusters, b.size*sizeof(double), sizeof(struct save_clusters_parameters), data_index_number_of_clusters, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &genus_array, &a_genus, b.size*sizeof(double), sizeof(struct save_clusters_parameters), data_index_genus, data_size)==1) return result_error;
			
			a_clusters->index=0;
			a_clusters->size=0;
			
			a_genus->index=0;
			a_genus->size=0;
			
			return result_continue;
		}
		case command_delete:
		{
			free (cluster_spins);
			free (cluster_list);
			free (lattice);
			return result_continue;
		}
		case command_run:
		{
			// Label the clusters.
			clusters (parameters, lattice, parameters->spins);

			// Get cluster statistics.
			cluster_stats (parameters, cluster_list, lattice, parameters->spins);

			// The lowest Euler characteristic among all the clusters is used to find
			// the genus of the lattice.
			//
			//     genus = 1 - (lowest Euler characteristic) / 2
			//
			
			// Initialize 'lowest_euler_characteristic'.			
			if (parameters->periodic_bc_x==0 && parameters->periodic_bc_y==0 && parameters->periodic_bc_z==0) lowest_euler_characteristic=2;
			else if (parameters->periodic_bc_x==1 && parameters->periodic_bc_y==1 && parameters->periodic_bc_z==1) lowest_euler_characteristic=0;
			
			// Initialize 'number_of_clusters'.
			number_of_clusters=0;
			
			// There must be at least one cluster.
			// 'number_of_clusters' gets incremented after each loop.
			i=0;
			do
			{
				// Get the spins that are in the cluster.
				save_cluster_spins (parameters, cluster_spins, i, cluster_list, lattice);
				
				// Get the topology of the cluster.
				if (parameters->periodic_bc_x==0 && parameters->periodic_bc_y==0 && parameters->periodic_bc_z==0)
				{
					topology_free_bc (parameters, &cluster_topology, cluster_spins);
				}
				else if (parameters->periodic_bc_x==1 && parameters->periodic_bc_y==1 && parameters->periodic_bc_z==1)
				{
					topology_periodic_bc (parameters, &cluster_topology, cluster_spins);
				}
				
				// Update 'lowest_euler_characteristic'.
				if (cluster_topology.euler_characteristic<lowest_euler_characteristic) lowest_euler_characteristic=cluster_topology.euler_characteristic;
				
				// Increment 'number_of_clusters'.
				number_of_clusters++;
			}
			while (i<nxtimesnytimesnz-1 && (cluster_list+(++i))->number_of_spins>0);

			// Determine the genus.
			genus=1-lowest_euler_characteristic/2;

			// Save the number of clusters.
			*(cluster_count+(a_clusters->index)++)=(double)number_of_clusters;
			a_clusters->size+=sizeof(double);

			// Save the genus.
			*(genus_array+(a_genus->index)++)=(double)genus;
			a_genus->size+=sizeof(double);

			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_check_same_genus(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static short int data_index;
	
	static double *same_genus;
	
	static int i, count, nxtimesnytimesnz;

	static int genus_1, genus_2;
	
	static int *lattice;
	static struct cluster_stats *cluster_list;
	static short int *cluster_spins;
	static struct cluster_topology cluster_topology;
	
	static struct save_value_data b;
	static struct save_value_parameters *a;
	
	switch (command)
	{
		case command_info: {printf("save_check_same_genus\n"); return result_continue;}
		case command_insert:
		{
			// Boundary conditions must be either free in all directions or
			// periodic in all directions.
			if (	!(parameters->periodic_bc_x==0 && parameters->periodic_bc_y==0 && parameters->periodic_bc_z==0) &&
				!(parameters->periodic_bc_x==1 && parameters->periodic_bc_y==1 && parameters->periodic_bc_z==1))
			{
				printf("Boundary conditions must be either free in all directions or periodic in all directions.\n");
				return result_error;
			}

			memcpy(&b, command_data, sizeof(struct save_clusters_data));
			
			nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);

			lattice=(int*)malloc (sizeof(int)*nxtimesnytimesnz);
			cluster_list=(struct cluster_stats*)malloc (sizeof(struct cluster_stats)*nxtimesnytimesnz);
			cluster_spins=(short int*)malloc (sizeof(short int)*nxtimesnytimesnz);
			
			return result_continue;
		}
		case command_begin:
		{
			if ((parameters->nx)*(parameters->ny)*(parameters->nz)!=nxtimesnytimesnz)
			{
				nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);

				lattice=(int*)realloc (lattice, sizeof(int)*nxtimesnytimesnz);
				cluster_list=(struct cluster_stats*)realloc (cluster_list, sizeof(struct cluster_stats)*nxtimesnytimesnz);
				cluster_spins=(short int*)realloc (cluster_spins, sizeof(short int)*nxtimesnytimesnz);
			}
			
			data_index=b.data_index;
			
			if (data_output(data, data_parameters, &same_genus, &a, b.size*sizeof(double), sizeof(struct save_clusters_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;
			
			return result_continue;
		}
		case command_delete:
		{
			free (cluster_spins);
			free (cluster_list);
			free (lattice);
			return result_continue;
		}
		case command_run:
		{
			// Label the clusters.
			clusters (parameters, lattice, parameters->spins);

			// Get cluster statistics.
			cluster_stats (parameters, cluster_list, lattice, parameters->spins);

			// The lowest Euler characteristic among all the clusters is used to find
			// the genus of the lattice.
			//
			//     genus = 1 - (lowest Euler characteristic) / 2
			//
			
			// There must be at least one cluster.
			// 'number_of_clusters' gets incremented after each loop.
			i=0;
			do
			{
				// Get the spins that are in the cluster.
				save_cluster_spins (parameters, cluster_spins, i, cluster_list, lattice);
				
				// Get the topology of the cluster.
				if (parameters->periodic_bc_x==0 && parameters->periodic_bc_y==0 && parameters->periodic_bc_z==0)
				{
					topology_free_bc (parameters, &cluster_topology, cluster_spins);
				}
				else if (parameters->periodic_bc_x==1 && parameters->periodic_bc_y==1 && parameters->periodic_bc_z==1)
				{
					topology_periodic_bc (parameters, &cluster_topology, cluster_spins);
				}
				
				if (i==0) genus_1=1-cluster_topology.euler_characteristic/2;
				if (i==1) genus_2=1-cluster_topology.euler_characteristic/2;
			}
			while (i<nxtimesnytimesnz-1 && (cluster_list+(++i))->number_of_spins>0);

			if (i!=2)
			{
				*(same_genus+(a->index)++)=2;
				a->size+=sizeof(double);
			}
			else
			{
				if (genus_1==genus_2)
				{
					*(same_genus+(a->index)++)=0;
					a->size+=sizeof(double);
				}
				else
				{
					*(same_genus+(a->index)++)=1;
					a->size+=sizeof(double);
				}
			}
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_final_energy(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index;
	
	static double *energy;

	static struct save_value_data b;
	static struct save_value_parameters *a;

	switch (command)
	{
		case command_info: {printf("save_final_energy\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_value_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index=b.data_index;
			
			if (data_output(data, data_parameters, &energy, &a, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;

			return result_continue;
		}
		case command_run:
		{
			*(energy+(a->index)++)=(double)parameters->energy;
			a->size+=sizeof(double);

			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_final_mag(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index;
	
	static double *mag;

	static struct save_value_data b;
	static struct save_value_parameters *a;

	switch (command)
	{
		case command_info: {printf("save_final_mag\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_value_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index=b.data_index;
			
			if (data_output(data, data_parameters, &mag, &a, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;

			return result_continue;
		}
		case command_run:
		{
			*(mag+(a->index)++)=(double)parameters->mag;
			a->size+=sizeof(double);

			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_final_time(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index;
	
	static double *time;

	static struct save_value_data b;
	static struct save_value_parameters *a;

	switch (command)
	{
		case command_info: {printf("save_final_time\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_value_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index=b.data_index;

			if (data_output(data, data_parameters, &time, &a, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;

			return result_continue;
		}
		case command_run:
		{
			*(time+(a->index)++)=parameters->time;
			a->size+=sizeof(double);

			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_lattice(short int command, struct save_lattice_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int num;
	static int prefix;

	static short int *spins_check;
	static short int *state_check;
	static int *map_check;
	static int *index_check;
	
	static int flip_spin_check;
	static int flip_check;
	static int energy_check;
	static int mag_check;
	static int active_check;
	static double time_check;
	static int steps_check;

	static unsigned long int seed_64;
	static short int *lattice_driven_up;
	static short int *lattice_driven_down;

	static FILE *up_file, *down_file;
	
	static char up_file_name[32]={}, down_file_name[32]={};

	static struct save_lattice_data b;
	static struct save_lattice_parameters *a;

	switch (command)
	{
		case command_info: {printf("save_lattice\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_lattice_data));
			
			num=b.num;
			prefix=b.prefix;
			
			return result_continue;
		}
		case command_begin:
		{
			lattice_driven_up=malloc (sizeof(short int)*(parameters->nx)*(parameters->ny)*(parameters->nz));
			lattice_driven_down=malloc (sizeof(short int)*(parameters->nx)*(parameters->ny)*(parameters->nz));

			state_check=(short int*)malloc(sizeof(short int)*(parameters->nx)*(parameters->ny)*(parameters->nz));
			map_check=(int*)malloc(sizeof(int)*(parameters->nx)*(parameters->ny)*(parameters->nz));
			index_check=(int*)malloc(sizeof(int)*(1+(parameters->nx)*(parameters->ny)*(parameters->nz)));

			return result_continue;
		}
		case command_run:
		{
			// Get the random number seed.
			seed_64=parameters->seed_64;

			// Get the current lattice.
			memcpy (lattice_driven_up, parameters->spins, sizeof(short int)*(parameters->nx)*(parameters->ny)*(parameters->nz));
			memcpy (lattice_driven_down, parameters->spins, sizeof(short int)*(parameters->nx)*(parameters->ny)*(parameters->nz));

			// Turn on the up infinitesimal bias magnetic field.
			evolve(0, 0, 0, 0, 0, parameters, lattice_driven_up, &flip_spin_check, &flip_check, state_check, map_check, index_check, &energy_check, &mag_check, &active_check, &time_check, &steps_check, 1, parameters->periodic_bc_x, parameters->periodic_bc_y, parameters->periodic_bc_z, parameters->nx, parameters->ny, parameters->nz, parameters->max_time, parameters->rng_seed, (parameters->rng)(), parameters->rng, parameters->rng_seed_64, (parameters->rng_64)(), parameters->rng_64, 0, 0);
			// Turn on the down infinitesimal bias magnetic field.
			evolve(0, 0, 0, 0, 0, parameters, lattice_driven_down, &flip_spin_check, &flip_check, state_check, map_check, index_check, &energy_check, &mag_check, &active_check, &time_check, &steps_check, -1, parameters->periodic_bc_x, parameters->periodic_bc_y, parameters->periodic_bc_z, parameters->nx, parameters->ny, parameters->nz, parameters->max_time, parameters->rng_seed, (parameters->rng)(), parameters->rng, parameters->rng_seed_64, (parameters->rng_64)(), parameters->rng_64, 0, 0);

			// Get the filenames for the up and down files.
			sprintf (up_file_name, "u-%d-%ld", prefix, seed_64);
			sprintf (down_file_name, "d-%d-%ld", prefix, seed_64);

			// Open the up and down files.
			up_file=fopen(up_file_name, "w"); down_file=fopen(down_file_name, "w");

			// Save the lattices.
			fwrite (lattice_driven_up, sizeof(short int), (parameters->nx)*(parameters->ny)*(parameters->nz), up_file);
			fwrite (lattice_driven_down, sizeof(short int), (parameters->nx)*(parameters->ny)*(parameters->nz), down_file);

			// Close the up and down files.
			fclose(down_file); fclose(up_file);

			return result_continue;
		}
		case command_end:
		{
			free (index_check);
			free (map_check);
			free (state_check);
			
			free (lattice_driven_down);
			free (lattice_driven_up);
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_number_of_blinker_regions(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int nxtimesnytimesnz;
	
	static short int *regions;
	
	static int num;
	
	static int data_index;
	
	static double *number_of_regions;

	static int *lattice;
	static struct cluster_stats *cluster_list;

	static struct save_value_data b;
	static struct save_value_parameters *a;

	switch (command)
	{
		case command_info: {printf("save_number_of_blinker_regions\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_value_data));

			nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);

			regions=(short int*)malloc (sizeof(short int)*nxtimesnytimesnz);
			lattice=(int*)malloc (sizeof(int)*nxtimesnytimesnz);
			cluster_list=(struct cluster_stats*)malloc (sizeof(struct cluster_stats)*nxtimesnytimesnz);

			return result_continue;
		}
		case command_begin:
		{
			if ((parameters->nx)*(parameters->ny)*(parameters->nz)!=nxtimesnytimesnz)
			{
				nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);

				regions=(short int*)realloc (regions, sizeof(short int)*nxtimesnytimesnz);
				lattice=(int*)realloc (lattice, sizeof(int)*nxtimesnytimesnz);
				cluster_list=(struct cluster_stats*)realloc (cluster_list, sizeof(struct cluster_stats)*nxtimesnytimesnz);
			}

			data_index=b.data_index;

			if (data_output(data, data_parameters, &number_of_regions, &a, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;

			return result_continue;
		}
		case command_run:
		{
			// Get the blinker regions.
			blinker_regions (parameters, data, data_parameters, data_size, 0, parameters->spins, regions, 0);

			// Label the clusters from the blinker regions.
			clusters (parameters, lattice, regions);

			// Get cluster statistics.
			cluster_stats (parameters, cluster_list, lattice, regions);

			// Determine the number of blinker regions.
			// The number of blinker regions equals the number of clusters found - 1.
			num=-1;
			while ((cluster_list+(++num)+1)->number_of_spins>0);
			
			// Save the number of blinker regions.
			*(number_of_regions+(a->index)++)=(double)num;
			a->size+=sizeof(double);

			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_percolation(short int command, struct save_perc_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static short int data_index_max_perc;
	static short int data_index_perc_x;
	static short int data_index_perc_y;
	static short int data_index_perc_z;
	
	static double *max_perc;
	static double *perc_x;
	static double *perc_y;
	static double *perc_z;
	
	static short int maximum_percolation;
	static short int percolation_in_x, percolation_in_y, percolation_in_z;
	
	static int i, count, nxtimesny, nxtimesnytimesnz;
	
	static int j, x, y, z, test_x, test_y, test_z;
	
	static short int *x_occupied, *y_occupied, *z_occupied;

	static int *lattice;
	static struct cluster_stats *cluster_list;
	static short int *cluster_spins;
	static struct cluster_topology cluster_topology;
	
	static struct save_perc_data b;
	static struct save_value_parameters *a_max_perc;
	static struct save_value_parameters *a_perc_x;
	static struct save_value_parameters *a_perc_y;
	static struct save_value_parameters *a_perc_z;
	
	switch (command)
	{
		case command_info: {printf("save_percolation\n"); return result_continue;}
		case command_insert:
		{
			// Boundary conditions must be either free in all directions or
			// periodic in all directions.
			if (	!(parameters->periodic_bc_x==0 && parameters->periodic_bc_y==0 && parameters->periodic_bc_z==0) &&
				!(parameters->periodic_bc_x==1 && parameters->periodic_bc_y==1 && parameters->periodic_bc_z==1))
			{
				printf("Boundary conditions must be either free in all directions or periodic in all directions.\n");
				return result_error;
			}

			memcpy(&b, command_data, sizeof(struct save_perc_data));
			
			nxtimesny=(parameters->nx)*(parameters->ny);
			nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);

			lattice=(int*)malloc (sizeof(int)*nxtimesnytimesnz);
			cluster_list=(struct cluster_stats*)malloc (sizeof(struct cluster_stats)*nxtimesnytimesnz);
			cluster_spins=(short int*)malloc (sizeof(short int)*nxtimesnytimesnz);
			
			x_occupied=(short int*)malloc (sizeof(short int)*(parameters->nx));
			y_occupied=(short int*)malloc (sizeof(short int)*(parameters->ny));
			z_occupied=(short int*)malloc (sizeof(short int)*(parameters->nz));
			
			return result_continue;
		}
		case command_begin:
		{
			if ((parameters->nx)*(parameters->ny)*(parameters->nz)!=nxtimesnytimesnz)
			{
				nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);

				lattice=(int*)realloc (lattice, sizeof(int)*nxtimesnytimesnz);
				cluster_list=(struct cluster_stats*)realloc (cluster_list, sizeof(struct cluster_stats)*nxtimesnytimesnz);
				cluster_spins=(short int*)realloc (cluster_spins, sizeof(short int)*nxtimesnytimesnz);
			}
			
			data_index_max_perc=b.data_index_max_perc;
			data_index_perc_x=b.data_index_perc_x;
			data_index_perc_y=b.data_index_perc_y;
			data_index_perc_z=b.data_index_perc_z;
			
			if (data_output(data, data_parameters, &max_perc, &a_max_perc, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index_max_perc, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &perc_x, &a_perc_x, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index_perc_x, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &perc_y, &a_perc_y, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index_perc_y, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &perc_z, &a_perc_z, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index_perc_z, data_size)==1) return result_error;
			
			a_max_perc->index=0;
			a_max_perc->size=0;
			
			a_perc_x->index=0;
			a_perc_x->size=0;
			
			a_perc_y->index=0;
			a_perc_y->size=0;
			
			a_perc_z->index=0;
			a_perc_z->size=0;
			
			return result_continue;
		}
		case command_delete:
		{
			free (z_occupied);
			free (y_occupied);
			free (x_occupied);
			free (cluster_spins);
			free (cluster_list);
			free (lattice);
			return result_continue;
		}
		case command_run:
		{
			// Label the clusters.
			clusters (parameters, lattice, parameters->spins);

			// Get cluster statistics.
			cluster_stats (parameters, cluster_list, lattice, parameters->spins);

			maximum_percolation=0;
			percolation_in_x=0; percolation_in_y=0; percolation_in_z=0;
			
			// Determine how each cluster percolates.
			i=0;
			do
			{
				test_x=0; test_y=0; test_z=0;
				
				// Get the spins that are in the cluster.
				save_cluster_spins (parameters, cluster_spins, i, cluster_list, lattice);
				
				// Initialize these arrays to zero.
				for (j=0; j<(parameters->nx); j++)
				{
					*(x_occupied+j)=0;
				}
				for (j=0; j<(parameters->ny); j++)
				{
					*(y_occupied+j)=0;
				}
				for (j=0; j<(parameters->nz); j++)
				{
					*(z_occupied+j)=0;
				}
				
				for (j=0; j<nxtimesnytimesnz; j++)
				{
					z=(int)(j/nxtimesny);
					y=(int)((j-z*nxtimesny)/(parameters->nx));
					x=(int)(j-z*nxtimesny-y*(parameters->nx));
					
					if (*(cluster_spins+j)==1)
					{
						if (*(x_occupied+x)==0) *(x_occupied+x)=1;
						if (*(y_occupied+y)==0) *(y_occupied+y)=1;
						if (*(z_occupied+z)==0) *(z_occupied+z)=1;
					}
				}
				
				for (j=0; j<(parameters->nx); j++)
				{
					if (*(x_occupied+j)==1) test_x++;
				}
				// If 'test_x' contains the maximum possible value, then
				// the cluster percolates in the x-direction. Set 'percolation_in_x'
				// to '1', and '1' gets written to the 'perc_x' array.
				// Otherwise, '0' gets written to the 'perc_x' array.
				//
				// The same analysis is done for the y and z directions.
				//
				if (percolation_in_x==0 && test_x==parameters->nx) percolation_in_x=1;

				for (j=0; j<(parameters->ny); j++)
				{
					if (*(y_occupied+j)==1) test_y++;
				}
				if (percolation_in_y==0 && test_y==parameters->ny) percolation_in_y=1;

				for (j=0; j<(parameters->nz); j++)
				{
					if (*(z_occupied+j)==1) test_z++;
				}
				if (percolation_in_z==0 && test_z==parameters->nz) percolation_in_z=1;
				
				// If this cluster percolates in more directions than any previous cluster,
				// update 'maximum_percolation'.
				if (maximum_percolation<3 &&
					(test_x==parameters->nx &&
					test_y==parameters->ny &&
					test_z==parameters->nz))
				{
					maximum_percolation=3;
				}
				else if (maximum_percolation<2 &&
					((test_x==parameters->nx &&
					test_y==parameters->ny) ||
					(test_y==parameters->ny &&
					test_z==parameters->nz) ||
					(test_z==parameters->nz &&
					test_x==parameters->nx)))
				{
					maximum_percolation=2;
				}
				else if (maximum_percolation<1 &&
					(test_x==parameters->nx ||
					test_y==parameters->ny ||
					test_z==parameters->nz))
				{
					maximum_percolation=1;
				}
			}
			while (i<nxtimesnytimesnz-1 && (cluster_list+(++i))->number_of_spins>0);

			// Save the maximum percolation among all clusters.
			*(max_perc+(a_max_perc->index)++)=(double)maximum_percolation;
			a_max_perc->size+=sizeof(double);

			// '1' gets written if one of the clusters percolates in the x direction; '0' otherwise.
			*(perc_x+(a_perc_x->index)++)=(double)percolation_in_x;
			a_perc_x->size+=sizeof(double);

			// '1' gets written if one of the clusters percolates in the y direction; '0' otherwise.
			*(perc_y+(a_perc_y->index)++)=(double)percolation_in_y;
			a_perc_y->size+=sizeof(double);

			// '1' gets written if one of the clusters percolates in the z direction; '0' otherwise.
			*(perc_z+(a_perc_z->index)++)=(double)percolation_in_z;
			a_perc_z->size+=sizeof(double);

			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_plot_blinker_regions(short int command, struct save_plotfbr_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int nxtimesnytimesnz;
	
	static int num;
	
	static int i, j;
	
	static int steps_begin;
	
	static short int *spins;
	static int *flips;
	static int *flips_size;
	static double *energy;
	static int *energy_size;
	
	static short int *regions;
	
	static FILE *data_file, *plot_file;
	
	static char data_file_name[6]={}, plot_file_name[6]={};
	
	static struct save_plotfbr_data b;
	static struct save_plotfbr_parameters *a;
	
	switch (command)
	{
		case command_info: {printf("save_plot_blinker_regions\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_plotfbr_data));
			
			nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);

			regions=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);

			return result_continue;
		}
		case command_begin:
		{
			if ((parameters->nx)*(parameters->ny)*(parameters->nz)!=nxtimesnytimesnz)
			{
				nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);
				regions=(short int*)realloc (regions, sizeof(short int)*nxtimesnytimesnz);
			}
			
			num=b.num;

			// Set 'i' to zero. 'i' is incremented after each run.
			// When i==num, no more animations are saved.
			i=0;
			
			return result_continue;
		}
		case command_run:
		{
			// Check if this realization's blinker regions are to be plotted.
			if (i<num)
			{
				// Save the blinker regions before the energy drop.
				blinker_regions (parameters, data, data_parameters, data_size, 0, parameters->spins, regions, 0);
				
				// Get the filenames for the data and plot files.
				sprintf (data_file_name, "d%d", i);
				sprintf (plot_file_name, "p%d", i);

				// Open the data and plot files.
				data_file=fopen(data_file_name, "w"); plot_file=fopen(plot_file_name, "w");

				// Plot the blinker regions.
				blinkers_plot (regions, parameters->nx, parameters->ny, parameters->nz, data_file_name, data_file, plot_file);

				// Close the data and plot files.
				fclose(data_file); fclose(plot_file);

				// Increment 'i'.
				i++;
			}

			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_pseudo_blinker_stats(short int command, struct save_pseudobl_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int nx;
	static int nxtimesny;
	static int nxtimesnytimesnz;
	
	static short int data_index_volume;
	static short int data_index_dim;
	static short int data_index_Ix;
	static short int data_index_Iy;
	static short int data_index_Iz;
	static int size;
	
	static int i, j;
	
	static int steps_begin;
	
	static short int *spins;
	
	static short int *regions_before_energy_drop, *regions_after_energy_drop, *regions;
	
	static double *brvol;
	static double *brdim;
	static double *Ix;
	static double *Iy;
	static double *Iz;
	
	static int number_of_spins;
	
	// These variables are initialized to '1' before checking the blinker region dimensionality.
	// If two blinker spins are found to have different x, y, or z coordinates, then 'same_x',
	// 'same_y', or 'same_z' is set to '0', respectively.
	static short int same_x, same_y, same_z;
	
	static int x_ref, y_ref, z_ref;
	static int x, y, z;
	
	static short int dimensions;
	
	static double x_com, y_com, z_com, moment_x, moment_y, moment_z;
	
	static struct save_pseudobl_data b;
	static struct save_value_parameters *a_volume, *a_dim, *a_Ix, *a_Iy, *a_Iz;
	
	switch (command)
	{
		case command_info: {printf("save_pseudo_blinker_stats\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_pseudobl_data));
			
			nx=parameters->nx;
			nxtimesny=nx*(parameters->ny);
			nxtimesnytimesnz=nxtimesny*(parameters->nz);

			spins=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			regions_before_energy_drop=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			regions_after_energy_drop=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			regions=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);

			return result_continue;
		}
		case command_begin:
		{
			if (parameters->nx!=nx || nx*(parameters->ny)!=nxtimesny || nxtimesny*(parameters->nz)!=nxtimesnytimesnz)
			{
				nx=parameters->nx;
				nxtimesny=nx*(parameters->ny);
				nxtimesnytimesnz=nxtimesny*(parameters->nz);
				
				spins=(short int*)realloc (spins, sizeof(short int)*nxtimesnytimesnz);
				regions_before_energy_drop=(short int*)realloc (regions_before_energy_drop, sizeof(short int)*nxtimesnytimesnz);
				regions_after_energy_drop=(short int*)realloc (regions_after_energy_drop, sizeof(short int)*nxtimesnytimesnz);
				regions=(short int*)realloc (regions, sizeof(short int)*nxtimesnytimesnz);
			}
			
			data_index_volume=b.data_index_volume;
			data_index_dim=b.data_index_dim;
			data_index_Ix=b.data_index_Ix;
			data_index_Iy=b.data_index_Iy;
			data_index_Iz=b.data_index_Iz;
			size=b.size;

			if (data_output(data, data_parameters, &brvol, &a_volume, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index_volume, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &brdim, &a_dim, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index_dim, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &Ix, &a_Ix, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index_Ix, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &Iy, &a_Iy, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index_Iy, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &Iz, &a_Iz, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index_Iz, data_size)==1) return result_error;
			
			a_volume->index=0;
			a_volume->size=0;
			a_dim->index=0;
			a_dim->size=0;
			a_Ix->index=0;
			a_Ix->size=0;
			a_Iy->index=0;
			a_Iy->size=0;
			a_Iz->index=0;
			a_Iz->size=0;

			return result_continue;
		}
		case command_run:
		{
			// Load the current lattice into 'spins'.
			memcpy(spins, parameters->spins, sizeof(short int)*nxtimesnytimesnz);

			// Save the blinker regions in the current lattice (which is after the final energy drop).
			blinker_regions (parameters, data, data_parameters, data_size, 0, spins, regions_after_energy_drop, 0);
			
			// Backtrack one step, and measure the blinker regions before the final energy drop.
			*(spins+parameters->flip_spin)=-*(spins+parameters->flip_spin);
			blinker_regions (parameters, data, data_parameters, data_size, 0, spins, regions_before_energy_drop, 0);
			
			// Determine the regions that were responsible for the long-lived
			// metastable state.
			// Also, count the number of pseudo-blinker spins.
			number_of_spins=0;
			for (j=0; j<nxtimesnytimesnz; j++)
			{
				*(regions+j)=*(regions_before_energy_drop+j)-*(regions_after_energy_drop+j);
				if (*(regions+j)!=0) number_of_spins++;
			}
			
			// Get the dimensionality of the blinker regions that were responsible for
			// the long-lived state.
			
			dimensions=0;
			same_x=1; same_y=1; same_z=1;
			
			for (i=0; i<nxtimesnytimesnz; i++)
			{
				// If the selected spin is a blinker spin, then exit this loop.
				// This spin will serve as a reference spin.
				// The coordinates of all other spins with be compared with this spin.
				if (*(regions+i)==1)
				{
					z_ref=(int)(i/nxtimesny);
					y_ref=(int)((i-z_ref*nxtimesny)/nx);
					x_ref=(int)(i-z_ref*nxtimesny-y_ref*nx);
					break;
				}
			}
			
			for (j=i+1; j<nxtimesnytimesnz; j++)
			{
				if (*(regions+j)==1)
				{
					z=(int)(j/nxtimesny);
					y=(int)((j-z*nxtimesny)/nx);
					x=(int)(j-z*nxtimesny-y*nx);
					
					if (same_x==1 && x!=x_ref) {same_x=0; dimensions++;}
					if (same_y==1 && y!=y_ref) {same_y=0; dimensions++;}
					if (same_z==1 && z!=z_ref) {same_z=0; dimensions++;}
				}
			}

			// Compute the moments of inertia about three orthogonal axes that pass through
			// the center of mass of the pseudo-blinker configuration.
			
			// Determine the center of mass of the pseudo-blinker configuration.
			x_com=0; y_com=0; z_com=0;
			for (j=0; j<nxtimesnytimesnz; j++)
			{
				z=(int)(j/nxtimesny);
				y=(int)((j-z*nxtimesny)/nx);
				x=(int)(j-z*nxtimesny-y*nx);

				if (*(regions+j)!=0) {x_com+=(double)(x+1); y_com+=(double)(y+1); z_com+=(double)(z+1);}
			}
			x_com/=(double)number_of_spins; y_com/=(double)number_of_spins; z_com/=(double)number_of_spins;
			
			// Determine the three moments of inertia about axes passing throught the center of mass.
			moment_x=0; moment_y=0; moment_z=0;
			for (j=0; j<nxtimesnytimesnz; j++)
			{
				z=(int)(j/nxtimesny);
				y=(int)((j-z*nxtimesny)/nx);
				x=(int)(j-z*nxtimesny-y*nx);
				
				if (*(regions+j)!=0)
				{
					moment_x+=pow(((double)y-y_com),2)+pow(((double)z-z_com),2);
					moment_y+=pow(((double)x-x_com),2)+pow(((double)z-z_com),2);
					moment_z+=pow(((double)x-x_com),2)+pow(((double)y-y_com),2);
				}
			}

			*(brvol+(a_volume->index)++)=(double)number_of_spins;
			a_volume->size+=sizeof(double);
			
			*(brdim+(a_dim->index)++)=(double)dimensions;
			a_dim->size+=sizeof(double);

			*(Ix+(a_Ix->index)++)=moment_x;
			a_Ix->size+=sizeof(double);

			*(Iy+(a_Iy->index)++)=moment_y;
			a_Iy->size+=sizeof(double);

			*(Iz+(a_Iz->index)++)=moment_z;
			a_Iz->size+=sizeof(double);

			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_seed(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index;
	
	static double *seed;

	static struct save_value_data b;
	static struct save_value_parameters *a;

	switch (command)
	{
		case command_info: {printf("save_seed\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_value_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index=b.data_index;

			if (data_output(data, data_parameters, &seed, &a, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;

			return result_continue;
		}
		case command_run:
		{
			*(seed+(a->index)++)=(double)parameters->seed;
			a->size+=sizeof(double);

			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_seed_64(short int command, struct save_seed_64_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index_low;
	static int data_index_high;
	
	static double *seed_64_low;
	static double *seed_64_high;

	static long int num2pow32=pow(2,16)*pow(2,16);

	static struct save_seed_64_data b;
	static struct save_seed_64_parameters *a1;
	static struct save_seed_64_parameters *a2;

	switch (command)
	{
		case command_info: {printf("save_seed_64\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_seed_64_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index_low=b.data_index_low;
			data_index_high=b.data_index_high;

			if (data_output(data, data_parameters, &seed_64_low, &a1, b.size*sizeof(double), sizeof(struct save_seed_64_parameters), data_index_low, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &seed_64_high, &a2, b.size*sizeof(double), sizeof(struct save_seed_64_parameters), data_index_high, data_size)==1) return result_error;
			
			a1->index=0;
			a1->size=0;
			
			a2->index=0;
			a2->size=0;

			return result_continue;
		}
		case command_run:
		{
			*(seed_64_low+(a1->index))=(double)((parameters->seed_64)%num2pow32);
			*(seed_64_high+(a2->index))=(double)((parameters->seed_64)/num2pow32);
			
			(a1->index)++;
			(a2->index)++;
			
			a1->size+=sizeof(double);
			a2->size+=sizeof(double);

			return result_continue;
		}
		default: return result_continue;
	}
}

short int save_steps(short int command, struct save_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index;
	
	static double *steps;

	static struct save_value_data b;
	static struct save_value_parameters *a;

	switch (command)
	{
		case command_info: {printf("save_steps\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct save_value_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index=b.data_index;

			if (data_output(data, data_parameters, &steps, &a, b.size*sizeof(double), sizeof(struct save_value_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;

			return result_continue;
		}
		case command_run:
		{
			*(steps+(a->index)++)=(double)parameters->steps;
			a->size+=sizeof(double);

			return result_continue;
		}
		default: return result_continue;
	}
}

