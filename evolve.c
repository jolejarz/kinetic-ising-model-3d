/**************************************************************
***************************************************************
*** 
***   evolve.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains the core calculation used by all routines for evolving the 3D Ising model
***   after a quench to zero temperature.
***
***************************************************************
**************************************************************/

#include "includes.h"

short int evolve(short int (*step[])(), short int step_size, void *data[], void *data_parameters[], short int data_size, struct evolve_parameters *parameters, short int *spins, int *flip_spin, int *flip, short int *state, int *map, int *index, int *energy, int *mag, int *active, double *time, int *steps, short int bias, short int periodic_bc_x, short int periodic_bc_y, short int periodic_bc_z, int nx, int ny, int nz, double max_time, int (*rng_seed)(), int seed, int (*rng)(), void (*rng_seed_64)(), unsigned long int seed_64, unsigned long int (*rng_64)(), short int force_flips, int *num_energy_drops)
{
	short int result, step_index=0, evolve_continue=1;
	
	// Temporary variables used for various purposes throughout the calculation.
	int a;
	int b;
	int c;
	int i;
	int j;
	int k;
	
	// Used to speed up the calculation.
	int tempindex1, tempindex2;

	// Contains the largest number that can be produced by the RNG plus 1.
	long double largest_random_number_plus_1;
	
	// Used as the x (spin1), y (spin2), and z (spin3) coordinates of the selected spin.
	int spin1;
	int spin2;
	int spin3;

	// 'flipped' is set to zero before each loop. If the selected spin is flipped, then 'flipped'
	// is set to one. This allows postprocessing later in the loop.
	int flipped=0;

	// Used in initializing 'state', 'map', 'index', 'e', and 'active'.
	int term1, term2, term3, term4, term5, term6;
	
	// Used to speed up the calculation.
	int nxtimesny=nx*ny;
	int nxtimesnytimesnz=nxtimesny*nz;

	// Used to keep track of which spins are no longer active after a spin has been flipped.
	// The 'map' and 'index' arrays are updated correspondingly after each spin flip.
	int holes[7]={};
	short int holes_count=0;
	
	// Used to force the lattice into its final state.
	short int flippable_spin;
	int energy_last;
	short int no_energy_drop=0;
	
	// Used to keep track of the energy on the previous step.
	int energy_last_value;
	
	// The largest random number that can be generated plus 1 is 2^64.
	largest_random_number_plus_1=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);
	largest_random_number_plus_1*=(long double)pow(2,16);

	// Initialize the random number generator.
	//rng_seed(seed);
	rng_seed_64 (seed_64);

	// Initialize the 'state', 'map', 'index', 'energy', 'mag', 'active', and 'time' arrays according to the
	// initial state of the lattice.
	*energy=0;
	*mag=0;
	b=1;
	for (k=0; k<=nz-1; k++)
	{
		for (j=0; j<=ny-1; j++)
		{
			for (i=0; i<=nx-1; i++)
			{
				c=*(spins+i+nx*j+nxtimesny*k);

				if (i<nx-1) term1=c*(*(spins+i+1+nx*j+nxtimesny*k));
				else term1=periodic_bc_x*c*(*(spins+nx*j+nxtimesny*k));
				if (j<ny-1) term2=c*(*(spins+i+nx*(j+1)+nxtimesny*k));
				else term2=periodic_bc_y*c*(*(spins+i+nxtimesny*k));
				if (k<nz-1) term3=c*(*(spins+i+nx*j+nxtimesny*(k+1)));
				else term3=periodic_bc_z*c*(*(spins+i+nx*j));
				if (i>0) term4=c*(*(spins+i-1+nx*j+nxtimesny*k));
				else term4=periodic_bc_x*c*(*(spins+nx-1+nx*j+nxtimesny*k));
				if (j>0) term5=c*(*(spins+i+nx*(j-1)+nxtimesny*k));
				else term5=periodic_bc_y*c*(*(spins+i+nx*(ny-1)+nxtimesny*k));
				if (k>0) term6=c*(*(spins+i+nx*j+nxtimesny*(k-1)));
				else term6=periodic_bc_z*c*(*(spins+i+nx*j+nxtimesny*(nz-1)));

				a=term1+term2+term3+term4+term5+term6;

				if (a<0 || (a==0 && (bias==0 || bias==-c)))
				{
					*(index+b)=i+nx*j+nx*ny*k;
					*(map+i+nx*j+nxtimesny*k)=b;
					b++;
				}
				else *(map+i+nx*j+nxtimesny*k)=0;

				*(state+i+nx*j+nxtimesny*k)=a;
				
				*energy-=a;
				*mag+=c;
			}
		}
	}
	*index=b-1;
	*active=*index;
	*time=0;
	*steps=0;

	// Update 'energy_last'.
	energy_last=*energy;
	
	// Update 'energy_last_value'.
	energy_last_value=*energy;
	
	// Set 'flippable_spin'.
	flippable_spin=(unsigned long int)rng_64()/largest_random_number_plus_1<0.5 ? -1 : 1;

	// If there are no active spins in the initial lattice, then there is nothing to do.
	if (*active==0) return result_continue;

	// Send 'begin' to all functions in 'step'.
	if (step!=0)
	{
		while (step[step_index]!=0 && step_index<step_size)
		{
			if (step[step_index++](command_begin, 0, data, data_parameters, data_size, parameters)==result_error) return result_error;
		}
		step_index=0;
	}

	// Begin the calculation.
	// The calculation will stop when
	// 	- the number of active spins reaches zero,
	//	- the maximum amount of allowed time has elapsed, or
	//	- a function in the 'step' array returns 'result_done'.
	do
	{
		// Randomly choose an index into the active spins array.
		
		j=1+(int)(*index*((unsigned long int)rng_64()/largest_random_number_plus_1));

		c=*(index+j);
		
		// Get the x, y, and z coordinates of the selected spin.
		spin3=(int)(c/nxtimesny);
		spin2=(int)((c-spin3*nxtimesny)/nx);
		spin1=(int)(c-spin3*nxtimesny-spin2*nx);

		// NOTE:
		//
		// If 'force_flips' is set to '0', then 'evolve' works normally.
		//
		// 'blinker_regions' may set it to '1' or '-1' to allow flips
		// only from - to + or from + to -, respectively. This ensures
		// correct behavior in determining the volume of blinker regions
		// if the lattice is not yet in its lowest energy state.
		//
		
		// Test if 'force_flips' is set to '0' (normal), or if flips are
		// only allowed in a certain direction.
		if (force_flips==0 || *(spins+c)==-force_flips)
		{
			// Test if the energy drops as a result of the flip.
			if (*(state+c)<0)
			{
			*(spins+c)=-*(spins+c);

				*(state+c)=-*(state+c);
				
				*energy-=2*(*(state+c));
				*mag+=2*(*(spins+c));
				
				flipped=1;
				
				*(index+j)=0;
				holes[holes_count++]=j;
				*(map+c)=0;
			}
			// Test if the energy stays the same as a result of the flip
			// and there is a bias field.
			else if (*(state+c)==0 && bias==-*(spins+c))
			{
				*(spins+c)=-*(spins+c);
				
				*mag+=2*(*(spins+c));
				
				flipped=1;
				
				*(index+j)=0;
				holes[holes_count++]=j;
				*(map+c)=0;
			}
			else if (*(state+c)==0 && bias==0)
			{
				// If there is no forced driving of zero-energy spins, then flip the selected
				// spin with probability 1/2.
				//
				// If there is forced driving of zero-energy spins, then flip the selected
				// spin if it has the same sign as 'flippable_spin'.
				//
				if (	(parameters->altbias==0 && (unsigned long int)rng_64()/largest_random_number_plus_1<0.5) ||
					((parameters->altbias==1 || parameters->altbias==2) && *(spins+c)==flippable_spin))
				{
					*(spins+c)=-*(spins+c);
					
					*mag+=2*(*(spins+c));
					
					flipped=1;
				}
			}
			*flip_spin=c;
			*flip=flipped;
		}
		
		// If the spin was flippped, do further processing.
		if (flipped==1)
		{
			// Update the energies of the six neighboring spins.
			if (spin1<nx-1)
			{
				*(state+spin1+1+nx*spin2+nxtimesny*spin3)+=2*(*(spins+c))*(*(spins+spin1+1+nx*spin2+nxtimesny*spin3));
				*energy-=2*(*(spins+c))*(*(spins+spin1+1+nx*spin2+nxtimesny*spin3));
			}
			else
			{
				*(state+nx*spin2+nxtimesny*spin3)+=periodic_bc_x*2*(*(spins+c))*(*(spins+nx*spin2+nxtimesny*spin3));
				*energy-=periodic_bc_x*2*(*(spins+c))*(*(spins+nx*spin2+nxtimesny*spin3));
			}
			if (spin2<ny-1)
			{
				*(state+spin1+nx*(spin2+1)+nxtimesny*spin3)+=2*(*(spins+c))*(*(spins+spin1+nx*(spin2+1)+nxtimesny*spin3));
				*energy-=2*(*(spins+c))*(*(spins+spin1+nx*(spin2+1)+nxtimesny*spin3));
			}
			else
			{
				*(state+spin1+nxtimesny*spin3)+=periodic_bc_y*2*(*(spins+c))*(*(spins+spin1+nxtimesny*spin3));
				*energy-=periodic_bc_y*2*(*(spins+c))*(*(spins+spin1+nxtimesny*spin3));
			}
			if (spin3<nz-1)
			{
				*(state+spin1+nx*spin2+nxtimesny*(spin3+1))+=2*(*(spins+c))*(*(spins+spin1+nx*spin2+nxtimesny*(spin3+1)));
				*energy-=2*(*(spins+c))*(*(spins+spin1+nx*spin2+nxtimesny*(spin3+1)));
			}
			else
			{
				*(state+spin1+nx*spin2)+=periodic_bc_z*2*(*(spins+c))*(*(spins+spin1+nx*spin2));
				*energy-=periodic_bc_z*2*(*(spins+c))*(*(spins+spin1+nx*spin2));
			}
			if (spin1>0)
			{
				*(state+spin1-1+nx*spin2+nxtimesny*spin3)+=2*(*(spins+c))*(*(spins+spin1-1+nx*spin2+nxtimesny*spin3));
				*energy-=2*(*(spins+c))*(*(spins+spin1-1+nx*spin2+nxtimesny*spin3));
			}
			else
			{
				*(state+nx-1+nx*spin2+nxtimesny*spin3)+=periodic_bc_x*2*(*(spins+c))*(*(spins+nx-1+nx*spin2+nxtimesny*spin3));
				*energy-=periodic_bc_x*2*(*(spins+c))*(*(spins+nx-1+nx*spin2+nxtimesny*spin3));
			}
			if (spin2>0)
			{
				*(state+spin1+nx*(spin2-1)+nxtimesny*spin3)+=2*(*(spins+c))*(*(spins+spin1+nx*(spin2-1)+nxtimesny*spin3));
				*energy-=2*(*(spins+c))*(*(spins+spin1+nx*(spin2-1)+nxtimesny*spin3));
			}
			else
			{
				*(state+spin1+nx*(ny-1)+nxtimesny*spin3)+=periodic_bc_y*2*(*(spins+c))*(*(spins+spin1+nx*(ny-1)+nxtimesny*spin3));
				*energy-=periodic_bc_y*2*(*(spins+c))*(*(spins+spin1+nx*(ny-1)+nxtimesny*spin3));
			}
			if (spin3>0)
			{
				*(state+spin1+nx*spin2+nxtimesny*(spin3-1))+=2*(*(spins+c))*(*(spins+spin1+nx*spin2+nxtimesny*(spin3-1)));
				*energy-=2*(*(spins+c))*(*(spins+spin1+nx*spin2+nxtimesny*(spin3-1)));
			}
			else
			{
				*(state+spin1+nx*spin2+nxtimesny*(nz-1))+=periodic_bc_z*2*(*(spins+c))*(*(spins+spin1+nx*spin2+nxtimesny*(nz-1)));
				*energy-=periodic_bc_z*2*(*(spins+c))*(*(spins+spin1+nx*spin2+nxtimesny*(nz-1)));
			}

			// Update the active spins array.
			for (a=0; a<6; a++)
			{
				if (a==0)
				{
					b=(nx-1)-spin1;
					c=periodic_bc_x;
					tempindex1=spin1+1+nx*spin2+nxtimesny*spin3;
					tempindex2=nx*spin2+nxtimesny*spin3;
				}
				else if (a==1)
				{
					b=(ny-1)-spin2;
					c=periodic_bc_y;
					tempindex1=spin1+nx*(spin2+1)+nxtimesny*spin3;
					tempindex2=spin1+nxtimesny*spin3;
				}
				else if (a==2)
				{
					b=(nz-1)-spin3;
					c=periodic_bc_z;
					tempindex1=spin1+nx*spin2+nxtimesny*(spin3+1);
					tempindex2=spin1+nx*spin2;
				}
				else if (a==3)
				{
					b=spin1;
					c=periodic_bc_x;
					tempindex1=spin1-1+nx*spin2+nxtimesny*spin3;
					tempindex2=nx-1+nx*spin2+nxtimesny*spin3;
				}
				else if (a==4)
				{
					b=spin2;
					c=periodic_bc_y;
					tempindex1=spin1+nx*(spin2-1)+nxtimesny*spin3;
					tempindex2=spin1+nx*(ny-1)+nxtimesny*spin3;
				}
				else if (a==5)
				{
					b=spin3;
					c=periodic_bc_z;
					tempindex1=spin1+nx*spin2+nxtimesny*(spin3-1);
					tempindex2=spin1+nx*spin2+nxtimesny*(nz-1);
				}
			
				if (b>0)
				{
					if (*(map+tempindex1)!=0 && (*(state+tempindex1)>0 || (*(state+tempindex1)==0 && bias==*(spins+tempindex1))))
					{
						*(index+*(map+tempindex1))=0;
						holes[holes_count++]=*(map+tempindex1);
						*(map+tempindex1)=0;
					}
					else if (*(map+tempindex1)==0 && (*(state+tempindex1)<0 || (*(state+tempindex1)==0 && (bias==0 || bias==-*(spins+tempindex1)))))
					{
						(*index)++;
						*(index+*index)=tempindex1;
						*(map+tempindex1)=*index;
					}
				}
				else if (c==1)
				{
					if (*(map+tempindex2)!=0 && (*(state+tempindex2)>0 || (*(state+tempindex2)==0 && bias==*(spins+tempindex2))))
					{
						*(index+*(map+tempindex2))=0;
						holes[holes_count++]=*(map+tempindex2);
						*(map+tempindex2)=0;
					}
					else if (*(map+tempindex2)==0 && (*(state+tempindex2)<0 || (*(state+tempindex2)==0 && (bias==0 || bias==-*(spins+tempindex2)))))
					{
						(*index)++;
						*(index+*index)=tempindex2;
						*(map+tempindex2)=*index;
					}
				}
			}

			// Fill in holes in the active spins array.
			holes_count--;
			while (holes_count>=0)
			{
				if (*(index+*index)!=0 || (*(index+*index)==0 && *map==*index))
				{
					*(index+holes[holes_count])=*(index+*index);
					*(map+*(index+*index))=holes[holes_count];
					*(index+*index)=0;
					holes[holes_count]=0;
				}
				else
				{
					for (a=0; a<6; a++)
					{
						if (holes[a]==*index)
						{
							for (b=a; b<6; b++)
							{
								holes[b]=holes[b+1];
							}
							break;
						}
					}
				}

				(*index)--;
				holes_count--;

				// If there are no more active spins, the simulation is done.
				if (*index==0) {evolve_continue=0; break;}

			}
		}

		// Do further processing if 'evolve' is running normally with no bias field.
		if (*index>0 && bias==0 && (parameters->altbias==1 || parameters->altbias==2))
		{
			if (*energy==energy_last)
			{
				// Check if no more spin flips are possible with the current value of 'flippable_spin'.
				// If a flip is possible on the next step, 'b' is set to '1'.
				b=0;
				for (a=0; a<*index; a++)
				{
					if (*(state+*(index+1+a))<0 || *(spins+*(index+1+a))==flippable_spin) {b=1; break;}
				}
				
				if (b==0)
				{
					// If the lattice has been driven in both directions with no energy drop,
					// end the simulation.
					if (no_energy_drop==1) evolve_continue=0;
					else
					{
						flippable_spin*=-1;
						no_energy_drop=1;
					}
				}
			}
			else if (*energy<energy_last)
			{
				// If 'parameters->altbias' is '1', the bias field changes sign.
				if (parameters->altbias==1) flippable_spin*=-1;
				// If 'parameters->altbias' is '2', the bias field is chosen randomly
				// with 1/2 probability up and 1/2 probability down.
				else flippable_spin=(unsigned long int)rng_64()/largest_random_number_plus_1<0.5 ? -1 : 1;

				no_energy_drop=0;
				energy_last=*energy;
			}
		}
		
		// If necessary, increment the number of energy drops.
		if (*energy<energy_last_value && num_energy_drops!=0) (*num_energy_drops)++;
		
		energy_last_value=*energy;
		
		holes_count=0;
		flipped=0;
		
		// Update the number of active spins and the time elapsed.
		*time+=1/(double)(*active);
		*active=*index;

		// Increment the number of steps elapsed.
		(*steps)++;

		// Do intermediate processing.
		if (step!=0)
		{
			while (step[step_index]!=0 && step_index<step_size)
			{
				result=step[step_index++](command_run, 0, data, data_parameters, data_size, parameters);
				if (result==result_error) return result_error;
				else if (result==result_done) {evolve_continue=0; break;}
			}
			step_index=0;
		}

		// If the time elapsed is greater than the maximum allowed time, end the calculation.
		if (parameters->altbias==0 && *time>max_time) evolve_continue=0;
	}
	while (evolve_continue);

	// Send 'end' to all functions in 'step'.
	if (step!=0)
	{
		while (step[step_index]!=0 && step_index<step_size)
		{
			if (step[step_index++](command_end, 0, data, data_parameters, data_size, parameters)==result_error) return result_error;
		}
		step_index=0;
	}
	
	return result_continue;
}

