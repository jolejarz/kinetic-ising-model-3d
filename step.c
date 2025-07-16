/**************************************************************
***************************************************************
*** 
***   step.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains the routines used by 'evolve' for intermediate processing.
***
***************************************************************
**************************************************************/

#include "includes.h"

short int step_avgtdrop(short int command, struct step_avgtdrop_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index_sum;
	static int data_index_num_times;
	static int size;
	
	static double *sum;
	static double *num_times;
	
	static struct step_avgtdrop_data b;
	
	static struct step_avgtdrop_parameters *a_sum;
	static struct step_avgtdrop_parameters *a_num_times;
	
	static int i;
	static int energy_last;
	static double time_last;
	
	switch (command)
	{
		case command_info: {printf("step_avgtdrop\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_avgtdrop_data));
			
			data_index_sum=b.data_index_sum;
			data_index_num_times=b.data_index_num_times;
			size=b.size;

			if (data_output(data, data_parameters, &sum, &a_sum, b.size*sizeof(double), sizeof(struct step_avgtdrop_parameters), data_index_sum, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &num_times, &a_num_times, b.size*sizeof(double), sizeof(struct step_avgtdrop_parameters), data_index_num_times, data_size)==1) return result_error;

			a_sum->index=0;
			a_sum->size=size*sizeof(double);
			
			a_num_times->index=0;
			a_num_times->size=size*sizeof(double);
			
			// Initialize memory to zero.
			memset (sum, 0, a_sum->size);
			memset (num_times, 0, a_num_times->size);
			
			return result_continue;
		}
		case command_begin:
		{
			i=0;
			
			energy_last=parameters->energy;
			time_last=0;
			
			return result_continue;
		}
		case command_run:
		{
			if ((parameters->energy)<energy_last)
			{
				*(sum+i)+=parameters->time-time_last;
				(*(num_times+i))++;
				
				energy_last=parameters->energy;
				time_last=parameters->time;
				
				i++;
			}
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int step_check(short int command, struct step_check_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static short int a;

	static int nx, ny, nz;
	static int nxtimesny;
	static int nxtimesnytimesnz;
	
	static int energy_last;

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
	
	static short int *energy_zero;
	static int energy_zero_count;
	
	static int i;
	
	static int x, y, z;
	static int xplus, xminus, yplus, yminus, zplus, zminus;

	static long double largest_random_number_plus_1;
	
	// The largest random number that can be generated plus 1 is 2^64.
	largest_random_number_plus_1=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);

	static struct step_check_data check_data;

	switch (command)
	{
		case command_info: {printf("step_check\n"); return result_continue;}
		case command_begin:
		{
			nx=parameters->nx; ny=parameters->ny; nz=parameters->nz;
			nxtimesny=nx*(parameters->ny);
			nxtimesnytimesnz=nxtimesny*(parameters->nz);
			
			// Allocate the memory used for final state checking.
			spins_check=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			state_check=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			map_check=(int*)malloc(sizeof(int)*nxtimesnytimesnz);
			index_check=(int*)malloc(sizeof(int)*(1+nxtimesnytimesnz));

			// Allocate the memory used for marking the spins with zero energy;
			energy_zero=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			
			// Initialize energy_last.
			energy_last=parameters->energy;
			
			// Loop through the lattice and determine which spins have zero energy.
			// Mark them off, and save the total number of zero-energy spins.
			// (0 = energy is zero; 1 = energy is nonzero)
			energy_zero_count=0;
			for (i=0; i<nxtimesnytimesnz; i++)
			{
				if (*(state+i)==0) {*(energy_zero+i)=0; energy_zero_count++;}
				else *(energy_zero+i)=1;
			}
			
			return result_continue;
		}
		case command_end:
		{
			// Free the memory used for final state checking.
			free(index_check);
			free(map_check);
			free(state_check);
			free(spins_check);
			
			// Free the memory used for marking spins.
			free(energy_zero);
			
			return result_continue;
		}
		case command_run:
		{
			// If a spin was flipped on the previous step, do further processing.
			if (parameters->flip==1)
			{
				// Get the x, y, and z coordinates of the flipped spin.
				z=(int)(parameters->flip_spin/nxtimesny);
				y=(int)((parameters->flip_spin-z*nxtimesny)/nx);
				x=(int)(parameters->flip_spin-z*nxtimesny-y*nx);

				// Determinine the coordinates of the neighboring spins.
				// If the boundaries are free along a direction and the flipped spin is at a boundary
				// where there is no neighboring spin, the coordinates of the missing neighbor are set
				// to the flipped spin's coordinates. Thus checking and updating the missing neighbor
				// results in no action, as the flipped spin has already been processed.
				if (x<nx-1) xplus=x+1; else xplus=parameters->periodic_bc_x==1 ? 0 : x;
				if (y<ny-1) yplus=y+1; else yplus=parameters->periodic_bc_y==1 ? 0 : y;
				if (z<nz-1) zplus=z+1; else zplus=parameters->periodic_bc_z==1 ? 0 : z;
				if (x>0) xminus=x-1; else xminus=parameters->periodic_bc_x==1 ? nx-1 : x;
				if (y>0) yminus=y-1; else yminus=parameters->periodic_bc_y==1 ? ny-1 : y;
				if (z>0) zminus=z-1; else zminus=parameters->periodic_bc_z==1 ? nz-1 : z;
				
				// Update the state of the flipped spin and its six neighboring spins.
				
				if (*(parameters->state+x+nx*y+nxtimesny*z)==0 && *(energy_zero+x+nx*y+nxtimesny*z)==1) {*(energy_zero+x+nx*y+nxtimesny*z)=0; energy_zero_count++;}
				if (*(parameters->state+xplus+nx*y+nxtimesny*z)==0 && *(energy_zero+xplus+nx*y+nxtimesny*z)==1) {*(energy_zero+xplus+nx*y+nxtimesny*z)=0; energy_zero_count++;}
				if (*(parameters->state+xminus+nx*y+nxtimesny*z)==0 && *(energy_zero+xminus+nx*y+nxtimesny*z)==1) {*(energy_zero+xminus+nx*y+nxtimesny*z)=0; energy_zero_count++;}
				if (*(parameters->state+x+nx*yplus+nxtimesny*z)==0 && *(energy_zero+x+nx*yplus+nxtimesny*z)==1) {*(energy_zero+x+nx*yplus+nxtimesny*z)=0; energy_zero_count++;}
				if (*(parameters->state+x+nx*yminus+nxtimesny*z)==0 && *(energy_zero+x+nx*yminus+nxtimesny*z)==1) {*(energy_zero+x+nx*yminus+nxtimesny*z)=0; energy_zero_count++;}
				if (*(parameters->state+x+nx*y+nxtimesny*zplus)==0 && *(energy_zero+x+nx*y+nxtimesny*zplus)==1) {*(energy_zero+x+nx*y+nxtimesny*zplus)=0; energy_zero_count++;}
				if (*(parameters->state+x+nx*y+nxtimesny*zminus)==0 && *(energy_zero+x+nx*y+nxtimesny*zminus)==1) {*(energy_zero+x+nx*y+nxtimesny*zminus)=0; energy_zero_count++;}

				if (*(parameters->state+x+nx*y+nxtimesny*z)!=0 && *(energy_zero+x+nx*y+nxtimesny*z)==0) {*(energy_zero+x+nx*y+nxtimesny*z)=1; energy_zero_count--;}
				if (*(parameters->state+xplus+nx*y+nxtimesny*z)!=0 && *(energy_zero+xplus+nx*y+nxtimesny*z)==0) {*(energy_zero+xplus+nx*y+nxtimesny*z)=1; energy_zero_count--;}
				if (*(parameters->state+xminus+nx*y+nxtimesny*z)!=0 && *(energy_zero+xminus+nx*y+nxtimesny*z)==0) {*(energy_zero+xminus+nx*y+nxtimesny*z)=1; energy_zero_count--;}
				if (*(parameters->state+x+nx*yplus+nxtimesny*z)!=0 && *(energy_zero+x+nx*yplus+nxtimesny*z)==0) {*(energy_zero+x+nx*yplus+nxtimesny*z)=1; energy_zero_count--;}
				if (*(parameters->state+x+nx*yminus+nxtimesny*z)!=0 && *(energy_zero+x+nx*yminus+nxtimesny*z)==0) {*(energy_zero+x+nx*yminus+nxtimesny*z)=1; energy_zero_count--;}
				if (*(parameters->state+x+nx*y+nxtimesny*zplus)!=0 && *(energy_zero+x+nx*y+nxtimesny*zplus)==0) {*(energy_zero+x+nx*y+nxtimesny*zplus)=1; energy_zero_count--;}
				if (*(parameters->state+x+nx*y+nxtimesny*zminus)!=0 && *(energy_zero+x+nx*y+nxtimesny*zminus)==0) {*(energy_zero+x+nx*y+nxtimesny*zminus)=1; energy_zero_count--;}
			}

			// If the energy dropped in the previous step and the number of active spins equals
			// the number of spins with zero energy, then check for the final state.
			if (energy_last>parameters->energy && energy_zero_count==*(parameters->index))
			{
				// The lattice is driven with an infinitesimal bias field in both the positive and negative directions.
				// 'a' here is the bias field that is used first.
				// Choose it to be + or - with equal probability.
				a=(unsigned long int)(parameters->rng_64)()/largest_random_number_plus_1<0.5 ? -1 : 1;

				// Initialize 'spins_check' for use in final state checking.
				memcpy(spins_check, parameters->spins, sizeof(short int)*nxtimesnytimesnz);

				// Impose the bias field and evolve the lattice until the number of active spins reaches zero.
				evolve(0, 0, 0, 0, 0, parameters, spins_check, &flip_spin_check, &flip_check, state_check, map_check, index_check, &energy_check, &mag_check, &active_check, &time_check, &steps_check, a, parameters->periodic_bc_x, parameters->periodic_bc_y, parameters->periodic_bc_z, parameters->nx, parameters->ny, parameters->nz, parameters->max_time, parameters->rng_seed, (parameters->rng)(), parameters->rng, parameters->rng_seed_64, (parameters->rng_64)(), parameters->rng_64, 0, 0);

				// Compare the initial and final energies.
				if (energy_check==parameters->energy)
				{
					// The initial and final energies are the same.
					// Reinitialize 'spins_check'.
					memcpy(spins_check, parameters->spins, sizeof(short int)*nxtimesnytimesnz);
				
					// Evolve the lattice with a bias field in the opposite direction.
					evolve(0, 0, 0, 0, 0, parameters, spins_check, &flip_spin_check, &flip_check, state_check, map_check, index_check, &energy_check, &mag_check, &active_check, &time_check, &steps_check, -a, parameters->periodic_bc_x, parameters->periodic_bc_y, parameters->periodic_bc_z, parameters->nx, parameters->ny, parameters->nz, parameters->max_time, parameters->rng_seed, (parameters->rng)(), parameters->rng, parameters->rng_seed_64, (parameters->rng_64)(), parameters->rng_64, 0, 0);

					// Compare the initial and final energies.
					// If equal, the system is assumed to be in its final state.
					if (energy_check==parameters->energy) return result_done;
				}
			}
		
			// Save the energy at the current step.
			energy_last=parameters->energy;
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int step_data_delete(short int command, struct step_data_delete_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static struct step_value_data b;
	
	static short int *indices_to_delete;
	
	static short int i, j;
	
	switch (command)
	{
		case command_info: {printf("step_data_delete\n"); return result_continue;}
		case command_insert:
		{
			memcpy (&b, command_data, sizeof(struct step_data_delete_data));
			
			// If it hasn't already been done by a previous call to this function,
			// allocate memory for the list of indices to be deleted each
			// time this function is called with 'command_begin'.
			if (indices_to_delete==0)
			{
				indices_to_delete=(short int*)malloc (sizeof(short int)*data_size);
				i=0;
			}
			
			// Add the data index to the list of data indices to be deleted.
			*(indices_to_delete+(i++))=b.data_index;
			
			return result_continue;
		}
		case command_begin:
		{
			// Delete all indices in 'indices_to_delete'.
			for (j=0; j<i; j++)
			{
				data_delete (data, data_parameters, *(indices_to_delete+j), data_size);
			}
			
			return result_continue;
		}
		case command_delete:
		{
			// Free the memory used by 'indices_to_delete'.
			free (indices_to_delete);
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int step_edrop(short int command, struct step_edrop_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index;
	
	static double *edrop;
	
	static struct step_edrop_data b;
	static struct step_edrop_parameters *a;
	
	static int edrop_index, energy_last;
	
	switch (command)
	{
		case command_info: {printf("step_edrop\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_edrop_data));
			
			edrop_index=0;
			
			data_index=b.data_index;

			if (data_output(data, data_parameters, &edrop, &a, b.size*sizeof(double), sizeof(struct step_edrop_parameters), data_index, data_size)==1) return result_error;

			return result_continue;
		}
		case command_begin:
		{
			a->index=edrop_index;
			a->size=(1+edrop_index)*sizeof(double);
			
			energy_last=parameters->energy;
			
			return result_continue;
		}
		case command_end:
		{
			edrop_index++;
			
			return result_continue;
		}
		case command_run:
		{
			if ((parameters->energy)<energy_last)
			{
				(*(edrop+(a->index)))++;
				energy_last=parameters->energy;
			}
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int step_flips(short int command, struct step_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index;
	
	static int *flips;
	
	static struct step_value_data b;
	static struct step_value_parameters *a;
	
	switch (command)
	{
		case command_info: {printf("step_flips\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_value_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index=b.data_index;
			
			if (data_output(data, data_parameters, &flips, &a, 2*b.steps*sizeof(int), sizeof(struct step_value_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;
			
			return result_continue;
		}
		case command_run:
		{
			*(flips+(a->index)++)=parameters->flip_spin;
			*(flips+(a->index)++)=parameters->flip;
			a->size+=2*sizeof(int);
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int step_force(short int command, struct step_force_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static short int a;

	static int nxtimesnytimesnz;
	
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
	
	static int end_time;

	static long double largest_random_number_plus_1;
	
	// The largest random number that can be generated plus 1 is 2^64.
	largest_random_number_plus_1=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);

	static struct step_check_data check_data;
	
	switch (command)
	{
		case command_info: {printf("step_force\n"); return result_continue;}
		case command_insert:
		{
			end_time=command_data->end_time;
			
			return result_continue;
		}
		case command_begin:
		{
			nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);
			
			// Allocate the memory used for final state checking.
			spins_check=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			state_check=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			map_check=(int*)malloc(sizeof(int)*nxtimesnytimesnz);
			index_check=(int*)malloc(sizeof(int)*(1+nxtimesnytimesnz));

			return result_continue;
		}
		case command_end:
		{
			// Free the memory used for final state checking.
			free(index_check);
			free(map_check);
			free(state_check);
			free(spins_check);
			
			return result_continue;
		}
		case command_run:
		{
			// If the system has evolved past the maximum time, drive it until
			// the energy drops.
			if ((parameters->time)>(parameters->max_time))
			{
				// The lattice is driven with an infinitesimal bias field in both the positive and negative directions.
				// 'a' here is the bias field that is used first.
				// Choose it to be + or - with equal probability.
				a=(unsigned long int)(parameters->rng_64)()/largest_random_number_plus_1<0.5 ? -1 : 1;

				// Initialize 'spins_check' for use in final state checking.
				memcpy(spins_check, parameters->spins, sizeof(short int)*nxtimesnytimesnz);

				// Impose the bias field and evolve the lattice until the number of active spins reaches zero.
				evolve(0, 0, 0, 0, 0, parameters, spins_check, &flip_spin_check, &flip_check, state_check, map_check, index_check, &energy_check, &mag_check, &active_check, &time_check, &steps_check, a, parameters->periodic_bc_x, parameters->periodic_bc_y, parameters->periodic_bc_z, parameters->nx, parameters->ny, parameters->nz, parameters->max_time, parameters->rng_seed, (parameters->rng)(), parameters->rng, parameters->rng_seed_64, (parameters->rng_64)(), parameters->rng_64, 0, 0);

				// Compare the initial and final energies.
				if (energy_check==parameters->energy)
				{
					// The initial and final energies are the same.
					// Reinitialize 'spins_check'.
					memcpy(spins_check, parameters->spins, sizeof(short int)*nxtimesnytimesnz);
				
					// Evolve the lattice with a bias field in the opposite direction.
					evolve(0, 0, 0, 0, 0, parameters, spins_check, &flip_spin_check, &flip_check, state_check, map_check, index_check, &energy_check, &mag_check, &active_check, &time_check, &steps_check, -a, parameters->periodic_bc_x, parameters->periodic_bc_y, parameters->periodic_bc_z, parameters->nx, parameters->ny, parameters->nz, parameters->max_time, parameters->rng_seed, (parameters->rng)(), parameters->rng, parameters->rng_seed_64, (parameters->rng_64)(), parameters->rng_64, 0, 0);
				}
				
				// The system is assumed to be in its final state.
				// Run the simulation a little longer to allow the blinker regions to fill in.
				evolve(0, 0, 0, 0, 0, parameters, spins_check, &flip_spin_check, &flip_check, state_check, map_check, index_check, &energy_check, &mag_check, &active_check, &time_check, &steps_check, 0, parameters->periodic_bc_x, parameters->periodic_bc_y, parameters->periodic_bc_z, parameters->nx, parameters->ny, parameters->nz, end_time, parameters->rng_seed, (parameters->rng)(), parameters->rng, parameters->rng_seed_64, (parameters->rng_64)(), parameters->rng_64, 0, 0);
				
				// Update the parameters.
				memcpy(parameters->spins, spins_check, sizeof(short int)*nxtimesnytimesnz);
				parameters->energy=energy_check;
				parameters->mag=mag_check;
				parameters->active=active_check;
				
				return result_done;
			}
		}
		default: return result_continue;
	}
}

short int step_force_end(short int command, struct step_forceend_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static short int num_energy_drops_index;
	static short int total_energy_drop_index;
	
	static double *num_energy_drops;
	static double *total_energy_drop;
	
	static short int a;

	static int nxtimesnytimesnz;
	
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
	
	static int end_time;

	static int energy_drop_count, energy_last;
	
	static long double largest_random_number_plus_1;
	
	// The largest random number that can be generated plus 1 is 2^64.
	largest_random_number_plus_1=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);

	static struct step_check_data check_data;

	static struct step_forceend_data b;
	static struct step_forceend_parameters *a_num_energy_drops;
	static struct step_forceend_parameters *a_total_energy_drop;
	
	switch (command)
	{
		case command_info: {printf("step_force_end\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_forceend_data));

			end_time=b.end_time;
			num_energy_drops_index=b.num_energy_drops_index;
			total_energy_drop_index=b.total_energy_drop_index;

			if (data_output(data, data_parameters, &num_energy_drops, &a_num_energy_drops, b.size*sizeof(double), sizeof(struct step_forceend_parameters), num_energy_drops_index, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &total_energy_drop, &a_total_energy_drop, b.size*sizeof(double), sizeof(struct step_forceend_parameters), total_energy_drop_index, data_size)==1) return result_error;

			a_num_energy_drops->index=0;
			a_num_energy_drops->size=0;
			
			a_total_energy_drop->index=0;
			a_total_energy_drop->size=0;
			
			return result_continue;
		}
		case command_begin:
		{
			nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);
			
			// Allocate the memory used for final state checking.
			spins_check=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			state_check=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			map_check=(int*)malloc(sizeof(int)*nxtimesnytimesnz);
			index_check=(int*)malloc(sizeof(int)*(1+nxtimesnytimesnz));

			*(num_energy_drops+(a_num_energy_drops->index))=0;
			*(total_energy_drop+(a_total_energy_drop->index))=0;

			return result_continue;
		}
		case command_end:
		{
			(a_num_energy_drops->index)++;
			(a_num_energy_drops->size)+=sizeof(double);
			
			(a_total_energy_drop->index)++;
			(a_total_energy_drop->size)+=sizeof(double);
				
			// Free the memory used for final state checking.
			free(index_check);
			free(map_check);
			free(state_check);
			free(spins_check);
			
			return result_continue;
		}
		case command_run:
		{
			// If the system has evolved past the maximum time, drive it until
			// the energy drops.
			if ((parameters->time)>(parameters->max_time))
			{
				// The lattice is driven with an infinitesimal bias field in both the positive and negative directions.
				// 'a' here is the bias field that is used first.
				// Choose it to be + or - with equal probability.
				a=(unsigned long int)(parameters->rng_64)()/largest_random_number_plus_1<0.5 ? -1 : 1;

				// Initialize 'spins_check' for use in final state checking.
				memcpy(spins_check, parameters->spins, sizeof(short int)*nxtimesnytimesnz);

				// Impose the bias field and evolve the lattice until the number of active spins reaches zero.
				energy_drop_count=0;
				evolve(0, 0, 0, 0, 0, parameters, spins_check, &flip_spin_check, &flip_check, state_check, map_check, index_check, &energy_check, &mag_check, &active_check, &time_check, &steps_check, a, parameters->periodic_bc_x, parameters->periodic_bc_y, parameters->periodic_bc_z, parameters->nx, parameters->ny, parameters->nz, parameters->max_time, parameters->rng_seed, (parameters->rng)(), parameters->rng, parameters->rng_seed_64, (parameters->rng_64)(), parameters->rng_64, 0, &energy_drop_count);

				// Update the number of energy drops.
				(*(num_energy_drops+(a_num_energy_drops->index)))+=energy_drop_count;
				
				energy_last=energy_check;
				
				// Reverse the sign of the bias field.
				a*=-1;
				
				// Drive the lattice in the opposite direction.
				energy_drop_count=0;
				evolve(0, 0, 0, 0, 0, parameters, spins_check, &flip_spin_check, &flip_check, state_check, map_check, index_check, &energy_check, &mag_check, &active_check, &time_check, &steps_check, a, parameters->periodic_bc_x, parameters->periodic_bc_y, parameters->periodic_bc_z, parameters->nx, parameters->ny, parameters->nz, parameters->max_time, parameters->rng_seed, (parameters->rng)(), parameters->rng, parameters->rng_seed_64, (parameters->rng_64)(), parameters->rng_64, 0, &energy_drop_count);

				// Update the number of energy drops.
				(*(num_energy_drops+(a_num_energy_drops->index)))+=energy_drop_count;
				
				// Compare the initial and final energies.
				while (energy_check<energy_last)
				{
					energy_last=energy_check;
					
					// Reverse the sign of the bias field.
					a*=-1;
					
					// Drive the lattice again.
					energy_drop_count=0;
					evolve(0, 0, 0, 0, 0, parameters, spins_check, &flip_spin_check, &flip_check, state_check, map_check, index_check, &energy_check, &mag_check, &active_check, &time_check, &steps_check, a, parameters->periodic_bc_x, parameters->periodic_bc_y, parameters->periodic_bc_z, parameters->nx, parameters->ny, parameters->nz, parameters->max_time, parameters->rng_seed, (parameters->rng)(), parameters->rng, parameters->rng_seed_64, (parameters->rng_64)(), parameters->rng_64, 0, &energy_drop_count);

					// Update the number of energy drops.
					(*(num_energy_drops+(a_num_energy_drops->index)))+=energy_drop_count;
				}

				// Save the total energy drop.
				(*(total_energy_drop+(a_total_energy_drop->index)))+=energy_check-parameters->energy;

				// The system is assumed to be in its final state.
				// Run the simulation a little longer to allow the blinker regions to fill in.
				evolve(0, 0, 0, 0, 0, parameters, spins_check, &flip_spin_check, &flip_check, state_check, map_check, index_check, &energy_check, &mag_check, &active_check, &time_check, &steps_check, 0, parameters->periodic_bc_x, parameters->periodic_bc_y, parameters->periodic_bc_z, parameters->nx, parameters->ny, parameters->nz, end_time, parameters->rng_seed, (parameters->rng)(), parameters->rng, parameters->rng_seed_64, (parameters->rng_64)(), parameters->rng_64, 0, 0);
				
				// Update the parameters.
				memcpy(parameters->spins, spins_check, sizeof(short int)*nxtimesnytimesnz);
				parameters->energy=energy_check;
				parameters->mag=mag_check;
				parameters->active=active_check;
				
				return result_done;
			}
		}
		default: return result_continue;
	}
}

short int step_force_end_a(short int command, struct step_forceenda_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static short int num_energy_drops_index;
	static short int total_energy_drop_index;
	
	static double *num_energy_drops;
	static double *total_energy_drop;
	
	static short int a;

	static int nxtimesnytimesnz;
	
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
	
	static int end_time;

	static int energy_drop_count, energy_last;
	
	static short int altbias;
	
	static long double largest_random_number_plus_1;
	
	// The largest random number that can be generated plus 1 is 2^64.
	largest_random_number_plus_1=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);

	static struct step_check_data check_data;

	static struct step_forceenda_data b;
	static struct step_forceenda_parameters *a_num_energy_drops;
	static struct step_forceenda_parameters *a_total_energy_drop;
	
	switch (command)
	{
		case command_info: {printf("step_force_end\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_forceenda_data));

			end_time=b.end_time;
			num_energy_drops_index=b.num_energy_drops_index;
			total_energy_drop_index=b.total_energy_drop_index;

			if (data_output(data, data_parameters, &num_energy_drops, &a_num_energy_drops, b.size*sizeof(double), sizeof(struct step_forceenda_parameters), num_energy_drops_index, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &total_energy_drop, &a_total_energy_drop, b.size*sizeof(double), sizeof(struct step_forceenda_parameters), total_energy_drop_index, data_size)==1) return result_error;

			a_num_energy_drops->index=0;
			a_num_energy_drops->size=0;
			
			a_total_energy_drop->index=0;
			a_total_energy_drop->size=0;
			
			return result_continue;
		}
		case command_begin:
		{
			nxtimesnytimesnz=(parameters->nx)*(parameters->ny)*(parameters->nz);
			
			// Allocate the memory used for final state checking.
			spins_check=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			state_check=(short int*)malloc(sizeof(short int)*nxtimesnytimesnz);
			map_check=(int*)malloc(sizeof(int)*nxtimesnytimesnz);
			index_check=(int*)malloc(sizeof(int)*(1+nxtimesnytimesnz));

			*(num_energy_drops+(a_num_energy_drops->index))=0;
			*(total_energy_drop+(a_total_energy_drop->index))=0;

			return result_continue;
		}
		case command_end:
		{
			(a_num_energy_drops->index)++;
			(a_num_energy_drops->size)+=sizeof(double);
			
			(a_total_energy_drop->index)++;
			(a_total_energy_drop->size)+=sizeof(double);
				
			// Free the memory used for final state checking.
			free(index_check);
			free(map_check);
			free(state_check);
			free(spins_check);
			
			return result_continue;
		}
		case command_run:
		{
			// If the system has evolved past the maximum time, drive it until
			// the energy drops.
			if ((parameters->time)>(parameters->max_time))
			{
				// Initialize 'spins_check' for use in final state checking.
				memcpy(spins_check, parameters->spins, sizeof(short int)*nxtimesnytimesnz);

				// Evolve the lattice with the alternating bias field until the number of active
				// spins reaches zero.
				altbias=parameters->altbias;
				parameters->altbias=1;
				energy_drop_count=0;
				evolve(0, 0, 0, 0, 0, parameters, spins_check, &flip_spin_check, &flip_check, state_check, map_check, index_check, &energy_check, &mag_check, &active_check, &time_check, &steps_check, 0, parameters->periodic_bc_x, parameters->periodic_bc_y, parameters->periodic_bc_z, parameters->nx, parameters->ny, parameters->nz, parameters->max_time, parameters->rng_seed, (parameters->rng)(), parameters->rng, parameters->rng_seed_64, (parameters->rng_64)(), parameters->rng_64, 0, &energy_drop_count);
				parameters->altbias=altbias;

				// Save the number of energy drops.
				(*(num_energy_drops+(a_num_energy_drops->index)))=energy_drop_count;
				
				// Save the total energy drop.
				(*(total_energy_drop+(a_total_energy_drop->index)))=energy_check-parameters->energy;

				// The system is assumed to be in its final state.
				// Run the simulation a little longer to allow the blinker regions to fill in.
				evolve(0, 0, 0, 0, 0, parameters, spins_check, &flip_spin_check, &flip_check, state_check, map_check, index_check, &energy_check, &mag_check, &active_check, &time_check, &steps_check, 0, parameters->periodic_bc_x, parameters->periodic_bc_y, parameters->periodic_bc_z, parameters->nx, parameters->ny, parameters->nz, end_time, parameters->rng_seed, (parameters->rng)(), parameters->rng, parameters->rng_seed_64, (parameters->rng_64)(), parameters->rng_64, 0, 0);
				
				// Update the parameters.
				memcpy(parameters->spins, spins_check, sizeof(short int)*nxtimesnytimesnz);
				parameters->energy=energy_check;
				parameters->mag=mag_check;
				parameters->active=active_check;
				
				return result_done;
			}
		}
		default: return result_continue;
	}
}

short int step_get_cutoff_time(short int command, struct step_getcutoff_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static short int final_energy_data_index;
	static int num_spins;
	static double frac_diff;
	static int num;
	
	static double *energies;
	static int *energies_size;
	
//	static int size, rank;
	
	static int cutoff_energy;
	
	static int i;
	
	static struct step_getcutoff_data b;
	static struct step_getcutoff_parameters *a;
	
	switch (command)
	{
		case command_info: {printf("step_get_cutoff_time\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_getcutoff_data));
			
			final_energy_data_index=b.final_energy_data_index;
			num_spins=b.num_spins;
			frac_diff=b.frac_diff;
			num=b.size;

			if (data_input(data, data_parameters, &energies, &energies_size, final_energy_data_index, data_size)==1) return result_error;

			// Get this process's rank.
//			MPI_Comm_rank(MPI_COMM_WORLD, &rank);

			// Get the total number of processes.
//			MPI_Comm_size(MPI_COMM_WORLD, &size);

			// Set the index into the list of energies.
			// This ensures that processes of the same rank always get the same energies.
			//
			// If this is the only process, then the list of energies must be read
			// starting at the beginning.
			//
//			if (size==1) i=0;
//			else i=(rank-1)*num;
			i=0;

			return result_continue;
		}
		case command_begin:
		{
			cutoff_energy=(int)((*(energies+(i++))+6*((double)num_spins))*(1+frac_diff));
			return result_continue;
		}
		case command_run:
		{
			if ((parameters->energy)+6*num_spins<cutoff_energy) return result_done;
			return result_continue;
		}
		default: return result_continue;
	}
}

short int step_intfcom(short int command, struct step_intfcom_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	// Saves the interface center of mass for an LxL blinker region.
	
	static int data_index;
	
	static double *intfcom;
	
	static struct step_intfcom_data b;
	static struct step_intfcom_parameters *a;
	
	switch (command)
	{
		case command_info: {printf("step_intfcom\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_intfcom_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index=b.data_index;
			
			if (data_output(data, data_parameters, &intfcom, &a, 1+b.steps*sizeof(double), sizeof(struct step_intfcom_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;

			*(intfcom+(a->index)++)=b.initial_com;
			a->size+=sizeof(double);

			return result_continue;
		}
		case command_run:
		{
			if (parameters->flip==1)
			{
				*(intfcom+(a->index)++)=*(intfcom+(a->index)-1)+(*(parameters->spins+parameters->flip_spin))*b.positive;
				a->size+=sizeof(double);
			}
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int step_lattice_rng(short int command, struct step_latrng_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int num;
	
	static int i;

//	static int size, rank;
	
	static unsigned long int rng_state;
	
	static FILE *spins_file, *rng_file;
	
	static char spins_file_name[20]={}, rng_file_name[20]={};

	static struct step_latrng_data b;

	switch (command)
	{
		case command_info: {printf("step_lattice_rng\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_latrng_data));

			num=b.num;
			
			// Get the number of processes.
//			MPI_Comm_size(MPI_COMM_WORLD, &size);
			
			// Get this process's rank.
//			MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//			if (size==1) i=0;
//			else i=(rank-1)*num;
			i=0;

			return result_continue;
		}
		case command_run:
		{
			// If the simulation evolved past the maximum time, save the lattice
			// and the state of the random number generator.
			//
			// The value for the state of the random number generator is used
			// as the seed if the simulation is continued from this point.
			//
			if ((parameters->time)>(parameters->max_time))
			{
				// Get the filenames for the spins and rng files.
				sprintf (spins_file_name, "s%d", i);
				sprintf (rng_file_name, "r%d", i);

				// Open the spins and rng files.
				spins_file=fopen(spins_file_name, "w"); rng_file=fopen(rng_file_name, "w");

				// Save the state of the lattice.
				fwrite (parameters->spins, sizeof(short int), (parameters->nx)*(parameters->ny)*(parameters->nz), spins_file);
				
				// Save the state of the random number generator.
				rng_state=parameters->rng_64_state();
				fwrite (&rng_state, sizeof(unsigned long int), 1, rng_file);
				
				// Close the data and plot files.
				fclose(rng_file); fclose(spins_file);

				i++;
				
				return result_done;
			}
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int step_long_drop(short int command, struct step_longdrop_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static short int data_index_step_low;
	static short int data_index_step_high;
	static int num;
	
	static long int num2pow32=pow(2,16)*pow(2,16);

	static double *step_low;
	static double *step_high;

	static int last_energy;

	static double last_time;
	static double max_time_difference;
	
	static long int current_step;
	static long int energy_drop_step;
	
	static struct step_longdrop_data b;
	static struct step_value_parameters *a_low;
	static struct step_value_parameters *a_high;

	switch (command)
	{
		case command_info: {printf("step_longdrop\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_longdrop_data));
			
			data_index_step_low=b.data_index_step_low;
			data_index_step_high=b.data_index_step_high;
			num=b.num;
			
			if (data_output(data, data_parameters, &step_low, &a_low, b.num*sizeof(double), sizeof(struct step_value_parameters), data_index_step_low, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &step_high, &a_high, b.num*sizeof(double), sizeof(struct step_value_parameters), data_index_step_high, data_size)==1) return result_error;
			
			a_low->index=0;
			a_low->size=0;
			a_high->index=0;
			a_high->size=0;
			
			return result_continue;
		}
		case command_begin:
		{
			// Initialize the arrays.
			*(step_low+(a_low->index))=0;
			*(step_high+(a_high->index))=0;

			// Initialize 'last_energy', 'last_time', 'current_step', and 'max_time_difference'.
			last_energy=parameters->energy;
			last_time=0;
			max_time_difference=0;
			current_step=1;
			
			return result_continue;
		}
		case command_run:
		{
			// If the energy did not drop on the previous step, increment
			// the number of constant-energy steps since the last energy
			// drop.
			//
			if (parameters->energy<last_energy)
			{
				// Check if the time elapsed is the maximum time
				// that has elapsed over any constant-energy series of steps.
				if ((parameters->time-last_time)>max_time_difference)
				{
					energy_drop_step=current_step;
					max_time_difference=parameters->time-last_time;

					// Update the arrays.
					*(step_low+(a_low->index))=(double)(energy_drop_step%num2pow32);
					*(step_high+(a_high->index))=(double)(energy_drop_step/num2pow32);
				}
				
				// Update 'last_energy' and 'last_time'.
				last_energy=parameters->energy;
				last_time=parameters->time;
			}
			
			current_step++;
			
			return result_continue;
		}
		case command_end:
		{
			// After each run, one additional number has been written.
			(a_low->index)++;
			a_low->size+=sizeof(double);
			(a_high->index)++;
			a_high->size+=sizeof(double);
		}

		default: return result_continue;
	}
}

short int step_mag_stop(short int command, struct step_magstop_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int mag;

	switch (command)
	{
		case command_info: {printf("step_mag_stop\n"); return result_continue;}
		case command_insert:
		{
			mag=command_data->mag;
			return result_continue;
		}
		case command_begin:
		{
			if (parameters->mag==mag) {printf("\n"); return result_error;}
			return result_continue;
		}
		case command_run:
		{
			if (parameters->mag==mag) return result_done;
			return result_continue;
		}
		default: return result_continue;
	}
}

short int step_save_active(short int command, struct step_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index;
	
	static double *active;

	static struct step_value_data b;
	static struct step_value_parameters *a;

	switch (command)
	{
		case command_info: {printf("step_active\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_value_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index=b.data_index;
			
			if (data_output(data, data_parameters, &active, &a, 1+b.steps*sizeof(double), sizeof(struct step_value_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;

			*(active+(a->index)++)=parameters->active;
			a->size+=sizeof(double);
			
			return result_continue;
		}
		case command_run:
		{
			*(active+(a->index)++)=parameters->active;
			a->size+=sizeof(double);

			return result_continue;
		}

		default: return result_continue;
	}
}

short int step_save_conste(short int command, struct step_svconste_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	short int data_index_step;
	short int data_index_previous_steps;
	short int data_index_time_elapsed;
	
	static double *step;
	static double *previous_steps;
	static double *time_elapsed;

	static double last_time;
	static double max_time_difference;
	
	static int energy_drop_step;
	static int number_of_constant_energy_steps;
	static int steps;
	static int last_energy;
	
	static struct step_svconste_data b;
	static struct step_svconste_parameters *a1;
	static struct step_svconste_parameters *a2;
	static struct step_svconste_parameters *a3;

	switch (command)
	{
		case command_info: {printf("step_save_conste\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_svconste_data));
			
			data_index_step=b.data_index_step;
			data_index_previous_steps=b.data_index_previous_steps;
			data_index_time_elapsed=b.data_index_time_elapsed;
			
			if (data_output(data, data_parameters, &step, &a1, b.num*sizeof(double), sizeof(struct step_svconste_parameters), data_index_step, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &previous_steps, &a2, b.num*sizeof(double), sizeof(struct step_svconste_parameters), data_index_previous_steps, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &time_elapsed, &a3, b.num*sizeof(double), sizeof(struct step_svconste_parameters), data_index_time_elapsed, data_size)==1) return result_error;
			
			a1->index=0;
			a1->size=0;
			a2->index=0;
			a2->size=0;
			a3->index=0;
			a3->size=0;
			
			return result_continue;
		}
		case command_begin:
		{
			// Initialize the arrays.
			*(step+(a1->index))=0;
			*(previous_steps+(a2->index))=0;
			*(time_elapsed+(a3->index))=0;

			// Initialize 'last_energy', 'last_time', 'steps', and 'max_time_difference'.
			last_energy=parameters->energy; last_time=0; steps=0; max_time_difference=0;
			
			return result_continue;
		}
		case command_run:
		{
			// If the energy did not drop on the previous step, increment
			// the number of constant-energy steps since the last energy
			// drop.
			//
			if (parameters->energy==last_energy) steps++;
			else
			{
				// Check if the time elapsed is the maximum time
				// that has elapsed over any constant-energy series of steps.
				if ((parameters->time-last_time)>max_time_difference)
				{
					energy_drop_step=parameters->steps;
					number_of_constant_energy_steps=steps;
					max_time_difference=parameters->time-last_time;

					// Update the arrays.
					*(step+(a1->index))=(double)energy_drop_step;
					*(previous_steps+(a2->index))=(double)number_of_constant_energy_steps;
					*(time_elapsed+(a3->index))=max_time_difference;
				}
				
				// Update 'steps', 'last_energy', and 'last_time'.
				steps=0;
				last_energy=parameters->energy;
				last_time=parameters->time;
			}
						
			return result_continue;
		}
		case command_end:
		{
			// After each run, one additional number has been saved to each of the
			// three arrays ('step', 'previous_steps', and 'time_elapsed').
			(a1->index)++;
			a1->size+=sizeof(double);
			(a2->index)++;
			a2->size+=sizeof(double);
			(a3->index)++;
			a3->size+=sizeof(double);
		}

		default: return result_continue;
	}
}

short int step_save_energy(short int command, struct step_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index;
	
	static double *energy;

	static struct step_value_data b;
	static struct step_value_parameters *a;

	switch (command)
	{
		case command_info: {printf("step_energy\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_value_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index=b.data_index;
			
			if (data_output(data, data_parameters, &energy, &a, 1+b.steps*sizeof(double), sizeof(struct step_value_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;

			*(energy+(a->index)++)=parameters->energy;
			a->size+=sizeof(double);
			
			return result_continue;
		}
		case command_run:
		{
			*(energy+(a->index)++)=parameters->energy;
			a->size+=sizeof(double);

			return result_continue;
		}
		default: return result_continue;
	}
}

short int step_save_mag(short int command, struct step_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index;
	
	static double *mag;

	static struct step_value_data b;
	static struct step_value_parameters *a;

	switch (command)
	{
		case command_info: {printf("step_mag\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_value_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index=b.data_index;
			
			if (data_output(data, data_parameters, &mag, &a, 1+b.steps*sizeof(double), sizeof(struct step_value_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;

			*(mag+(a->index)++)=parameters->mag;
			a->size+=sizeof(double);
			
			return result_continue;
		}
		case command_run:
		{
			*(mag+(a->index)++)=parameters->mag;
			a->size+=sizeof(double);

			return result_continue;
		}
		default: return result_continue;
	}
}

short int step_save_time(short int command, struct step_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index;
	
	static double *time;

	static struct step_value_data b;
	static struct step_value_parameters *a;

	switch (command)
	{
		case command_info: {printf("step_time\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_value_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index=b.data_index;
			
			if (data_output(data, data_parameters, &time, &a, 1+b.steps*sizeof(double), sizeof(struct step_value_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;

			*(time+(a->index)++)=parameters->time;
			a->size+=sizeof(double);
			
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

short int step_save_time_drop(short int command, struct step_value_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index;
	
	static double *time_drop;

	static struct step_value_data b;
	static struct step_value_parameters *a;

	static int energy_last;
	
	switch (command)
	{
		case command_info: {printf("step_time_drop\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_value_data));
			
			return result_continue;
		}
		case command_begin:
		{
			data_index=b.data_index;
			
			if (data_output(data, data_parameters, &time_drop, &a, 1+b.steps*sizeof(double), sizeof(struct step_value_parameters), data_index, data_size)==1) return result_error;
			
			a->index=0;
			a->size=0;

			energy_last=parameters->energy;

			return result_continue;
		}
		case command_run:
		{
			if ((parameters->energy)<energy_last)
			{
				*(time_drop+(a->index)++)=parameters->time;
				a->size+=sizeof(double);
				energy_last=parameters->energy;
			}

			return result_continue;
		}
		default: return result_continue;
	}
}

short int step_stop(short int command, struct step_stop_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int steps;

	switch (command)
	{
		case command_info: {printf("step_stop\n"); return result_continue;}
		case command_insert:
		{
			steps=command_data->steps;
			return result_continue;
		}
		case command_begin:
		{
			if (steps==0) {printf("\n"); return result_error;}
			return result_continue;
		}
		case command_run:
		{
			if (parameters->steps==steps) return result_done;
			return result_continue;
		}
		default: return result_continue;
	}
}

short int step_stop_long(short int command, struct step_stop_long_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static short int step_data_index_low;
	static short int step_data_index_high;
	
	static double *steps_low;
	static int *steps_low_size;
	static double *steps_high;
	static int *steps_high_size;

	static int i;
	static unsigned long int current_step, stop_step;

//	static int rank, size;
	
	static int num;
	
	switch (command)
	{
		case command_info: {printf("step_stop_long\n"); return result_continue;}
		case command_insert:
		{
			step_data_index_low=command_data->step_data_index_low;
			step_data_index_high=command_data->step_data_index_high;
			num=command_data->num;

			if (data_input(data, data_parameters, &steps_low, &steps_low_size, step_data_index_low, data_size)==1) return result_error;
			if (data_input(data, data_parameters, &steps_high, &steps_high_size, step_data_index_high, data_size)==1) return result_error;

			// Get this process's rank.
//			MPI_Comm_rank(MPI_COMM_WORLD, &rank);

			// Get the total number of processes.
//			MPI_Comm_size(MPI_COMM_WORLD, &size);
			
			// Set the index into the list of steps.
			//
			// If this is the only process, then the list of steps must be read
			// starting at the beginning.
			//
//			if (size==1) i=0;
//			else i=(rank-1)*num;
			i=0;

			return result_continue;
		}
		case command_begin:
		{
			stop_step=(unsigned long int)(*(steps_high+i));
			stop_step*=(unsigned long int)pow(2,16);
			stop_step*=(unsigned long int)pow(2,16);
			stop_step+=(unsigned long int)(*(steps_low+i));

			current_step=1;
			
			return result_continue;
		}
		case command_run:
		{
			if (current_step==stop_step)
			{
				i++;
				return result_done;
			}
			else current_step++;
			
			return result_continue;
		}
		default: return result_continue;
	}
}

short int step_time_drop_vs_time(short int command, struct step_tdropvst_data *command_data, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters)
{
	static int data_index_times;
	static int data_index_drop_times;
	static int size;
	
	static double *times;
	static double *drop_times;
	
	static struct step_tdropvst_data b;
	
	static struct step_tdropvst_parameters *a_times;
	static struct step_tdropvst_parameters *a_drop_times;
	
	static int i;
	static int energy_last;
	static double time_last;
	
	switch (command)
	{
		case command_info: {printf("step_tdropvst\n"); return result_continue;}
		case command_insert:
		{
			memcpy(&b, command_data, sizeof(struct step_tdropvst_data));
			
			data_index_times=b.data_index_times;
			data_index_drop_times=b.data_index_drop_times;
			size=b.size;

			if (data_output(data, data_parameters, &times, &a_times, b.size*sizeof(double), sizeof(struct step_tdropvst_parameters), data_index_times, data_size)==1) return result_error;
			if (data_output(data, data_parameters, &drop_times, &a_drop_times, b.size*sizeof(double), sizeof(struct step_tdropvst_parameters), data_index_drop_times, data_size)==1) return result_error;

			a_times->index=0;
			a_times->size=size*sizeof(double);
			
			a_drop_times->index=0;
			a_drop_times->size=size*sizeof(double);
			
			// Initialize memory to zero.
			memset (times, 0, a_times->size);
			memset (drop_times, 0, a_drop_times->size);
			
			i=0;
			
			return result_continue;
		}
		case command_begin:
		{
			energy_last=parameters->energy;
			time_last=0;
			
			return result_continue;
		}
		case command_run:
		{
			if ((parameters->energy)<energy_last)
			{
				*(times+i)=time_last;
				*(drop_times+i)=parameters->time-time_last;
				
				energy_last=parameters->energy;
				time_last=parameters->time;
				
				i++;
			}
			
			return result_continue;
		}
		default: return result_continue;
	}
}

