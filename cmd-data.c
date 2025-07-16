/**************************************************************
***************************************************************
*** 
***   cmd-data.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains commands for manipulating data.
***
***************************************************************
**************************************************************/

void cmd_abs()
{
	short int data_index_input=arg_short_int[0];
	short int data_index_output=arg_short_int[1];

	double *input, *output;
	int *input_size, *output_size;
	
	if (data_input(data, data_parameters, &input, &input_size, data_index_input, data_num)==1) return;
	if (data_output(data, data_parameters, &output, &output_size, *input_size, sizeof(int), data_index_output, data_num)==1) return;
	
	*output_size=*input_size;
	
	int data_number=*input_size/sizeof(double);
	
	int i;
	
	for (i=0; i<data_number; i++)
	{
		*(output+i)=abs(*(input+i));
	}

	return;
}

void cmd_add()
{
	short int data_index_input=arg_short_int[0];
	short int data_index_output=arg_short_int[1];
	double number=arg_double[0];

	double *input, *output;
	int *input_size, *output_size;
	
	if (data_input(data, data_parameters, &input, &input_size, data_index_input, data_num)==1) return;
	if (data_output(data, data_parameters, &output, &output_size, *input_size, sizeof(int), data_index_output, data_num)==1) return;
	
	*output_size=*input_size;
	
	int data_number=*input_size/sizeof(double);
	
	int i;
	
	for (i=0; i<data_number; i++)
	{
		*(output+i)=*(input+i)+number;
	}

	return;
}

void cmd_avg()
{
	short int data_index=arg_short_int[0];
	
	double *data_list_double;
	
	int *data_list_parameters;
	
	int data_number, i;
	
	double sum=0;
	
	if (data_input(data, data_parameters, &data_list_double, &data_list_parameters, data_index, data_num)==1) return;
	data_number=*data_list_parameters/sizeof(double);

	for (i=0; i<data_number; i++)
	{
		sum+=*(data_list_double+i);
	}
	
	printf("%.10lf\n", sum/(double)data_number);
	
	return;
}

void cmd_bins()
{
	short int data_index=arg_short_int[0];
	int bin_size=arg_int[0];
	
	double *data_list_double;
	
	int *data_list_parameters;
	
	int data_number, i, j, num_bins;
	
	double *bin_averages;
	
	double average, stdev, sum_sq;
	
	if (data_input(data, data_parameters, &data_list_double, &data_list_parameters, data_index, data_num)==1) return;
	data_number=*data_list_parameters/sizeof(double);
	
	if (data_number%bin_size!=0) {printf ("The data set must be able to be broken into an integer number of bins.\n"); return;}
	num_bins=data_number/bin_size;
	bin_averages=malloc (sizeof(double)*data_number/bin_size);
	
	for (j=0; j<num_bins; j++)
	{
		*(bin_averages+j)=0;
		for (i=0; i<bin_size; i++)
		{
			*(bin_averages+j)+=*(data_list_double+j*bin_size+i);
		}
		*(bin_averages+j)/=(double)bin_size;
	}
	
	average=0; sum_sq=0;
	for (j=0; j<num_bins; j++)
	{
		average+=*(bin_averages+j);
		sum_sq+=pow(*(bin_averages+j),2);
	}
	average/=(double)num_bins;
	stdev=sqrt(sum_sq/(double)num_bins-pow(average,2));
	
	printf ("bin averages = %.10lf,  standard error = %.10lf\n", average, stdev/sqrt(num_bins));
	
	free (bin_averages);
	
	return;
}

void cmd_breg()
{
	short int data_index_flips=arg_short_int[0];
	short int data_index_regions=arg_short_int[1];
	int plot_steps=arg_int[0];
	
	short int *output;
	int *output_size;

	if (data_output(data, data_parameters, &output, &output_size, sizeof(short int)*parameters.nx*parameters.ny*parameters.nz, sizeof(int), data_index_regions, data_num)==1) return;

	*output_size=sizeof(short int)*parameters.nx*parameters.ny*parameters.nz;

	blinker_regions(&parameters, data, data_parameters, data_num, data_index_flips, parameters.spins_initial, output, plot_steps);
	
	return;
}

void cmd_cmplat()
{
	short int data_index_seeds_low=arg_short_int[0];
	short int data_index_seeds_high=arg_short_int[1];
	int first_prefix=arg_int[0];
	int second_prefix=arg_int[1];
	int num_spins=arg_int[2];
	int num=arg_int[3];

	unsigned long int num2pow32=pow(2,16)*pow(2,16);
	
	unsigned long int seed_64;

	double *seeds_low;
	double *seeds_high;
	int *seeds_low_parameters;
	int *seeds_high_parameters;
	
	if (data_input(data, data_parameters, &seeds_low, &seeds_low_parameters, data_index_seeds_low, data_num)==1) return;
	if (data_input(data, data_parameters, &seeds_high, &seeds_high_parameters, data_index_seeds_high, data_num)==1) return;

	short int *first_lattice=malloc (sizeof(short int)*num_spins);
	short int *second_lattice=malloc (sizeof(short int)*num_spins);

	int i, j;
	
	int num_diff=0, diff_sites, total_diff_sites=0;
	
	FILE *first_lattice_file, *second_lattice_file;
	
	char first_lattice_file_name[32]={}, second_lattice_file_name[32]={};

	for (i=0; i<num; i++)
	{
		// Get the seed.
		seed_64=(unsigned int)(*(seeds_high+i))*num2pow32+(unsigned int)(*(seeds_low+i));

		// Get the filenames for the lattices to be compared.
		sprintf (first_lattice_file_name, "u-%d-%ld", first_prefix, seed_64);
		sprintf (second_lattice_file_name, "u-%d-%ld", second_prefix, seed_64);

		// Open the lattice files.
		first_lattice_file=fopen(first_lattice_file_name, "r"); second_lattice_file=fopen(second_lattice_file_name, "r");

		// Read the lattices.
		fread (first_lattice, sizeof(short int), num_spins, first_lattice_file);
		fread (second_lattice, sizeof(short int), num_spins, second_lattice_file);

		diff_sites=0;
		for (j=0; j<num_spins; j++)
		{
			if (*(first_lattice+j)!=*(second_lattice+j)) diff_sites++;
		}
		if (diff_sites>0)
		{
			total_diff_sites+=diff_sites;
			printf ("seed=%ld, number of differing lattice sites=%d\n", seed_64, diff_sites);
			num_diff++;
		}
		
		fclose (second_lattice_file);
		fclose (first_lattice_file);
	}
	
	printf("\n");
	printf("%d pairs of lattices were different.\n", num_diff);
	printf("average number of differing lattice sites per realization=%lf\n",total_diff_sites/(double)num/(double)num_spins);
	printf("\n");
	
	free (second_lattice);
	free (first_lattice);
	
	return;
}

void cmd_cnt()
{
	short int data_index=arg_short_int[0];
	double min=arg_double[0];
	double max=arg_double[1];
	
	double *data_list_double;
	
	int *data_list_parameters;
	
	int data_number, i;
	
	int count=0;
	
	if (data_input(data, data_parameters, &data_list_double, &data_list_parameters, data_index, data_num)==1) return;
	data_number=*data_list_parameters/sizeof(double);

	for (i=0; i<data_number; i++)
	{
		if (*(data_list_double+i)>=min && *(data_list_double+i)<=max) count++;
	}
	
	printf("%d\n", count);
	
	return;
}

void cmd_conste()
{
	short int energy_index=arg_short_int[0];
	short int time_index=arg_short_int[1];
	short int flips_index=arg_short_int[2];
	
	double *data_energy, *data_time;
	int *data_flips;
	
	int *energy_size, *time_size, *flips_size;

	int data_number;
	
	if (data_input(data, data_parameters, &data_energy, &energy_size, energy_index, data_num)==1) return;
	
	data_number=*energy_size/sizeof(double);

	if (data_input(data, data_parameters, &data_time, &time_size, time_index, data_num)==1) return;

	if (data_input(data, data_parameters, &data_flips, &flips_size, flips_index, data_num)==1) return;

	int i, j=0, max=0, step=0, x, y, z;
	double time;
	
	// Determine the maximum number of constant energy steps that elapsed.
	for (i=1; i<=parameters.steps; i++)
	{
		if (*(data_energy+i)==*(data_energy+i-1)) j++;
		else
		{
			if (j>max)
			{
				max=j;
				step=i;
			}
			j=0;
		}
	}

	z=(int)(*(data_flips+2*(step-1))/parameters.nx/parameters.ny);
	y=(int)((*(data_flips+2*(step-1))-z*parameters.nx*parameters.ny)/parameters.nx);
	x=(int)(*(data_flips+2*(step-1))-z*parameters.nx*parameters.ny-y*parameters.nx);

	time=*(data_time+step)-*(data_time+step-max);

	printf("duration of the longest constant-energy metastable state (real time) = %lf\n", time);
	printf("                                             number of steps elapsed = %d\n", max);
	printf("                                    step at which the energy dropped = %d\n", step);
	printf("    coordinates of the spin that caused the energy to drop (x, y, z) = (%d, %d, %d)\n", x+1, y+1, z+1);

	return;
}

void cmd_dd()
{
	data_delete(data, data_parameters, arg_short_int[0], data_num);
	return;
}

void cmd_dindex()
{
	// Delete all entries from the parallel index list
	parallel_index(0, 0, 0, data_num);
	return;
}

void cmd_disp()
{
	short int data_index=arg_short_int[0];
	int min=arg_int[0];
	int max=arg_int[1];
	
	double *data_list_double;
	
	void *data_list_parameters;
	
	if (data_input(data, data_parameters, &data_list_double, &data_list_parameters, data_index, data_num)==1) return;
	
	int i;
	
	for (i=min; i<=max; i++)
	{
		printf("%lf\n", *(data_list_double+i));
	}
	
	return;
}

void cmd_div()
{
	short int data_index_input=arg_short_int[0];
	short int data_index_output=arg_short_int[1];
	double number=arg_double[0];

	double *input, *output;
	int *input_size, *output_size;
	
	if (data_input(data, data_parameters, &input, &input_size, data_index_input, data_num)==1) return;
	if (data_output(data, data_parameters, &output, &output_size, *input_size, sizeof(int), data_index_output, data_num)==1) return;
	
	*output_size=*input_size;
	
	int data_number=*input_size/sizeof(double);
	
	int i;
	
	for (i=0; i<data_number; i++)
	{
		*(output+i)=*(input+i)/number;
	}

	return;
}

void cmd_divarray()
{
	short int data_index_input_numerator=arg_short_int[0];
	short int data_index_input_denominator=arg_short_int[1];
	short int data_index_output=arg_short_int[2];

	double *input_numerator, *input_denominator, *output;
	int *input_numerator_size, *input_denominator_size, *output_size;
	
	if (data_input(data, data_parameters, &input_numerator, &input_numerator_size, data_index_input_numerator, data_num)==1) return;
	if (data_input(data, data_parameters, &input_denominator, &input_denominator_size, data_index_input_denominator, data_num)==1) return;
	if (data_output(data, data_parameters, &output, &output_size, *input_numerator_size, sizeof(int), data_index_output, data_num)==1) return;
	
	*output_size=*input_numerator_size;
	
	int data_number=*input_numerator_size/sizeof(double);
	
	int i;
	
	for (i=0; i<data_number; i++)
	{
		*(output+i)=*(input_numerator+i)/(*(input_denominator+i));
	}

	return;
}

void cmd_dt()
{
	// Delete all entries from the tag list
	tag(0, 0, 0, data_num);
	return;
}

void cmd_filerng()
{
	FILE *rngfile=fopen (arg_string[0], "r");
	
	fread (&parameters.seed_64, sizeof(unsigned long int), 1, rngfile);
	
	fclose (rngfile);
	
	return;
}

void cmd_filespins()
{
	FILE *spinsfile=fopen (arg_string[0], "r");
	
	fread (parameters.spins_initial, sizeof(short int), parameters.nx*parameters.ny*parameters.nz, spinsfile);

	fclose (spinsfile);
	
	return;
}

void cmd_flt()
{
	short int data_index=arg_short_int[0];
	double min=arg_double[0];
	double max=arg_double[1];

	double *input[data_num/2], *output[data_num/2];
	int *input_size[data_num/2], *output_size[data_num/2];
	
	struct tag_data a=tag(2, 0, 0, data_num);

	int i, j, k, output_index=0, count=0;

	for (i=0; i<data_num/2; i++)
	{
		if (a.data_input[i]!=a.data_output[i])
		{
			if (data_input(data, data_parameters, &input[i], &input_size[i], a.data_input[i], data_num)==1) return;
			if (data_output(data, data_parameters, &output[i], &output_size[i], *input_size[i], sizeof(int), a.data_output[i], data_num)==1) return;

			*output_size[i]=*input_size[i];
			
			if (a.data_input[i]==data_index) j=i;
			
			count++;
		}
		else break;
	}
	
	for (i=1; i<count; i++)
	{
		if (*input_size[i]!=*input_size[i-1])
		{
			printf("The sizes of all tagged input arrays must be the same.\n");
			return;
		}
	}
	
	int data_number_input=*input_size[j]/sizeof(double);
	
	for (i=0; i<data_number_input; i++)
	{
		if (*(input[j]+i)>=min && *(input[j]+i)<=max)
		{
			for (k=0; k<count; k++)
			{
				*(output[k]+output_index)=*(input[k]+i);
			}
			output_index++;
		}
	}
	
	int data_number_output=output_index*sizeof(double);
	
	for (i=0; i<count; i++)
	{
		*output_size[i]=data_number_output;
	}
	
	printf("%d numbers were written to each of the output data blocks.\n", output_index);

	return;
}

void cmd_index()
{
	short int data_index=arg_short_int[0];
	int data_size=arg_int[0];

	// Add the data block to the index list for transfer in a parallel computation
	parallel_index(1, data_index, data_size, data_num);

	return;
}

void cmd_initstat()
{
	int size=(parameters.nx)*(parameters.ny)*(parameters.nz);
	
	printf("initial lattice:\n");
	printf("\n");
	printf("  energy          = %d\n",parameters.energy);
	printf("  energy per spin = %lf\n",(parameters.energy)/(double)size);
	printf("  mag             = %d\n",parameters.mag);
	printf("  mag per spin    = %lf\n",(parameters.mag)/(double)size);
	printf("\n");
	
	return;
}

void cmd_log10()
{
	short int data_index_input=arg_short_int[0];
	short int data_index_output=arg_short_int[1];
	
	double *input, *output;
	int *input_size, *output_size;
	
	if (data_input(data, data_parameters, &input, &input_size, data_index_input, data_num)==1) return;
	if (data_output(data, data_parameters, &output, &output_size, *input_size, sizeof(int), data_index_output, data_num)==1) return;
	
	*output_size=*input_size;
	
	int data_number=*input_size/sizeof(double);
	
	int i;
	
	for (i=0; i<data_number; i++)
	{
		*(output+i)=log10(*(input+i));
	}
	
	return;
}

void cmd_minmax()
{
	short int data_index=arg_short_int[0];

	double *input;
	int *input_size;
	int data_number;
	
	if (data_input(data, data_parameters, &input, &input_size, data_index, data_num)==1) return;
	data_number=*input_size/sizeof(double);

	double min;
	double max;
	
	int i;
	
	min=*input;
	max=*input;
	for (i=1; i<data_number; i++)
	{
		if (*(input+i)<min) min=*(input+i);
		if (*(input+i)>max) max=*(input+i);
	}

	printf("min=%lf, max=%lf\n", min, max);
	
	return;
}

void cmd_mul()
{
	short int data_index_input=arg_short_int[0];
	short int data_index_output=arg_short_int[1];
	double number=arg_double[0];

	double *input, *output;
	int *input_size, *output_size;
	
	if (data_input(data, data_parameters, &input, &input_size, data_index_input, data_num)==1) return;
	if (data_output(data, data_parameters, &output, &output_size, *input_size, sizeof(int), data_index_output, data_num)==1) return;
	
	*output_size=*input_size;
	
	int data_number=*input_size/sizeof(double);
	
	int i;
	
	for (i=0; i<data_number; i++)
	{
		*(output+i)=*(input+i)*number;
	}

	return;
}

void cmd_prob()
{
	short int data_index_input=arg_short_int[0];
	short int data_index_output_x=arg_short_int[1];
	short int data_index_output_y=arg_short_int[2];
	double min=arg_double[0];
	double max=arg_double[1];
	int bins=arg_int[0];
	int num=arg_int[1];
	
	int *input_size;
	int data_number;
	
	double *input;

	int i;

	if (data_input(data, data_parameters, &input, &input_size, data_index_input, data_num)==1) return;
	data_number=*input_size/sizeof(double);
		
	double *x, *y;
	int *x_size, *y_size;

	if (data_output(data, data_parameters, &x, &x_size, bins*sizeof(double), sizeof(int), data_index_output_x, data_num)==1) return;
	if (data_output(data, data_parameters, &y, &y_size, bins*sizeof(double), sizeof(int)+sizeof(double), data_index_output_y, data_num)==1) return;

	*x_size=bins*sizeof(double); *y_size=bins*sizeof(double);

	double *bin_width=y_size+sizeof(int);
	
	*bin_width=(max-min)/(double)bins;

	prob_dist(input, min, max, data_number, num, bins, x, y);
	
	return;
}

void cmd_rdm()
{
	short int data_index_input=arg_short_int[0];
	double number=arg_double[0];

	double *input;
	int *input_size;
	
	if (data_input(data, data_parameters, &input, &input_size, data_index_input, data_num)==1) return;
	
	int data_number=*input_size/sizeof(double);
	
	int i;
	
	double j=0;
	
	for (i=0; i<data_number; i++)
	{
		j+=pow(*(input+i), number);
	}
	j/=(double)data_number;
	
	printf("%lf\n", pow(j, 1/number));

	return;
}

void cmd_receive()
{
/*
	// If this process is the master (rank 0), then receive all data from the slaves.
	
	int size;
	int rank;
	
	int i, j;
	
	// This structure contains info on all the data blocks that are transferred from the slaves.
	struct parallel_index_data index_data=parallel_index(2, 0, 0, data_num);
	
	// This array contains pointers to the data blocks that the transferred data are put into.
	static double *all_data[data_num];
	static struct save_value_parameters *all_data_parameters[data_num];
	
	// Get the number of processes.
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	// Get this process's rank.
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	// If not the master, or if there is only one process, then don't do any processing here.
	if (rank>0 || size==1) return;
	
	// Create the storage space for the transferred data.
	for (i=0; i<index_data.num; i++)
	{
		if (data_output(data, data_parameters, &all_data[i], &all_data_parameters[i], (size-1)*index_data.data_size[i], sizeof(struct save_value_parameters), index_data.data_index[i], data_num)==1) return;
	}
	
	for (j=0; j<index_data.num; j++)
	{
		all_data_parameters[j]->size=0;
	}

	// Get the data from each slave process.
	for (i=1; i<size; i++)
	{
		for (j=0; j<index_data.num; j++)
		{
			MPI_Recv(	data[index_data.data_index[j]]+(i-1)*index_data.data_size[j],
					index_data.data_size[j]/sizeof(double),
					MPI_DOUBLE,
					i,
					0,
					MPI_COMM_WORLD,
					MPI_STATUS_IGNORE
				);
			all_data_parameters[j]->size+=index_data.data_size[j];
		}
		printf("Received all data from process %d.\n", i);
	}
*/	
	return;
}

void cmd_sd()
{
	int i;
	
	for (i=0; i<data_num; i++)
	{
		printf("%d  ", i);
		if (data[i]!=0) printf("occupied\n"); else printf("free\n");
	}
	
	return;
}

void cmd_send()
{
/*
	// If this process is a slave (rank>0), then send its data to the master.
	
	int rank;
	
	int i;
	
	// This structure contains info on all the data blocks that are transferred from the slaves.
	struct parallel_index_data index_data=parallel_index(2, 0, 0, data_num);

	// Get this process's rank.
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	// If not a slave, then don't do any processing here.
	if (rank==0) return;
	
	// Send all data to the master.
	for (i=0; i<index_data.num; i++)
	{
		MPI_Ssend(	data[index_data.data_index[i]],
				index_data.data_size[i]/sizeof(double),
				MPI_DOUBLE,
				0,
				0,
				MPI_COMM_WORLD
			);
	}
	
	// Exit this process.
	MPI_Finalize();
	exit(0);
*/	
	return;
}

void cmd_stdev()
{
	short int data_index=arg_short_int[0];
	
	double *data_list_double;
	
	int *data_list_parameters;
	
	int data_number, i;
	
	double x, sum_x=0, sum_x_sq=0, avg_x, avg_x_sq, stdev;
	
	if (data_input(data, data_parameters, &data_list_double, &data_list_parameters, data_index, data_num)==1) return;
	data_number=*data_list_parameters/sizeof(double);

	for (i=0; i<data_number; i++)
	{
		x=*(data_list_double+i);
		sum_x+=x;
		sum_x_sq+=x*x;
	}
	avg_x=sum_x/(double)data_number;
	avg_x_sq=sum_x_sq/(double)data_number;
	stdev=sqrt(avg_x_sq-avg_x*avg_x);
	
	printf("%.10lf\n", stdev);
	
	return;
}

void cmd_subarray()
{
	short int data_index_input_first=arg_short_int[0];
	short int data_index_input_second=arg_short_int[1];
	short int data_index_output=arg_short_int[2];

	double *input_first, *input_second, *output;
	int *input_first_size, *input_second_size, *output_size;
	
	if (data_input(data, data_parameters, &input_first, &input_first_size, data_index_input_first, data_num)==1) return;
	if (data_input(data, data_parameters, &input_second, &input_second_size, data_index_input_second, data_num)==1) return;
	if (data_output(data, data_parameters, &output, &output_size, *input_first_size, sizeof(int), data_index_output, data_num)==1) return;
	
	*output_size=*input_first_size;
	
	int data_number=*input_first_size/sizeof(double);
	
	int i;
	
	for (i=0; i<data_number; i++)
	{
		*(output+i)=*(input_first+i)-(*(input_second+i));
	}

	return;
}

void cmd_surv()
{
	short int data_index_input=arg_short_int[0];
	short int data_index_output=arg_short_int[1];

	int i;
	
	double *input;
	int *input_size;
	
	int data_number;

	if (data_input(data, data_parameters, &input, &input_size, data_index_input, data_num)==1) return;

	data_number=*input_size/sizeof(double);

	double *output;
	int *output_size;

	if (data_output(data, data_parameters, &output, &output_size, *input_size, sizeof(int), data_index_output, data_num)==1) return;
	
	*output_size=*input_size;

	memcpy(output, input, *input_size);

	double *bin_width=input_size+sizeof(int);

	*output=1-(*output)*(*bin_width);
	for (i=1; i<data_number; i++)
	{
		*(output+i)=*(output+i-1)-(*(output+i))*(*bin_width);
	}
	
	return;
}

void cmd_szd()
{
	short int data_index=arg_short_int[0];

	double *input;
	int *input_size;
	
	if (data_input(data, data_parameters, &input, &input_size, data_index, data_num)==1) return;

	printf("%d\n", *input_size);
	
	return;
}

void cmd_tag()
{
	short int data_index_input=arg_short_int[0];
	short int data_index_output=arg_short_int[1];
	
	// Add the data block to the tag list.
	tag(1, data_index_input, data_index_output, data_num);
	
	return;
}

void cmd_tcollect()
{
	// If this is a slave process, do not do any calculation.
//	int rank;
//	int size;
//	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//	MPI_Comm_size(MPI_COMM_WORLD, &size);
//	if (rank>0) return;
	
	short int data_index_old_sum=arg_short_int[0];
	short int data_index_old_num_times=arg_short_int[1];
	short int data_index_new_sum=arg_short_int[2];
	short int data_index_new_num_times=arg_short_int[3];
	int max_drops=arg_int[0];
	int num_processes=arg_int[1];
	
	double *old_sum, *old_num_times, *new_sum, *new_num_times;
	
	int *old_sum_size, *old_num_times_size, *new_sum_size, *new_num_times_size;
	
	int i;
	int data_number;
	
	if (data_input(data, data_parameters, &old_sum, &old_sum_size, data_index_old_sum, data_num)==1) return;
	if (data_input(data, data_parameters, &old_num_times, &old_num_times_size, data_index_old_num_times, data_num)==1) return;
	if (data_output(data, data_parameters, &new_sum, &new_sum_size, max_drops*sizeof(double), sizeof(struct step_avgtdrop_parameters), data_index_new_sum, data_num)==1) return;
	if (data_output(data, data_parameters, &new_num_times, &new_num_times_size, max_drops*sizeof(double), sizeof(struct step_avgtdrop_parameters), data_index_new_num_times, data_num)==1) return;

	data_number=*old_sum_size/sizeof(double);

	memset (new_sum, 0, max_drops*sizeof(double));
	memset (new_num_times, 0, max_drops*sizeof(double));
	
	for (i=0; i<data_number; i++)
	{
		*(new_sum+i%max_drops)+=*(old_sum+i);
		*(new_num_times+i%max_drops)+=*(old_num_times+i);
	}
	*new_sum_size=max_drops*sizeof(double);
	*new_num_times_size=max_drops*sizeof(double);
	
	return;
}

void cmd_text()
{
	// All data in the specified data block is written in text format to the output file.
	
	short int data_index=arg_short_int[0];

	double *input;
	int *input_size;
	
	if (data_input(data, data_parameters, &input, &input_size, data_index, data_num)==1) return;

	FILE *datafile=fopen(arg_string[0], "w");
	
	int i;
	
	for (i=0; i<(*input_size)/sizeof(double); i++)
	{
		fprintf(datafile, "%lf\n", *(input+i));
	}
	
	fclose(datafile);
	
	printf("%d numbers were written to %s.\n", (*input_size)/sizeof(double), arg_string[0]);
	
	return;
}

