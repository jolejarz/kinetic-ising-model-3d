/**************************************************************
***************************************************************
*** 
***   cmd-set.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains commands for setting parameters.
***
***************************************************************
**************************************************************/

void cmd_af()
{
	set_antiferro(arg_short_int[0], &parameters);
	return;
}

void cmd_altbias()
{
	set_altbias(arg_short_int[0], &parameters);
	return;
}

void cmd_bc()
{
	set_bc(arg_short_int[0], arg_short_int[1], arg_short_int[2], &parameters);
	return;
}

void cmd_bias()
{
	set_bias(arg_short_int[0], &parameters);
	return;
}

void cmd_blkinit()
{
	// Creates an initial lattice of blocks with alternating spins.
	// The parameter passed is the linear size of each block.
	// The first block (starting at (x,y,z)=(1,1,1)) is filled with up spins.
	
	if (parameters.nx!=parameters.ny || parameters.ny!=parameters.nz || parameters.nz!=parameters.nx)
	{
		printf("The lattice must be square to use 'blkinit'.\n");
		return;
	}
	
	int size=parameters.nx;
	int block_size=arg_int[0];

	if (size%(2*block_size)!=0)
	{
		printf("An integer number of up/down block combinations must fit within the linear size of the lattice.\n");
		return;
	}
		
	int x,y,z;
	int spin=1;
	
	for (z=1; z<=size; z+=block_size)
	{
		for (y=1; y<=size; y+=block_size)
		{
			for (x=1; x<=size; x+=block_size)
			{
				set_block(spin, x, x+block_size-1, y, y+block_size-1, z, z+block_size-1, &parameters);
				spin*=-1;
			}
			spin*=-1;
		}
		spin*=-1;
	}
	
	return;
}

void cmd_block()
{
	set_block(arg_short_int[0], arg_int[0], arg_int[1], arg_int[2], arg_int[3], arg_int[4], arg_int[5], &parameters);
	return;
}

void cmd_ev()
{
	cmd_bias();
	arg_short_int[0]=arg_short_int[1];
	arg_short_int[1]=arg_short_int[2];
	arg_short_int[2]=arg_short_int[3];
	cmd_bc();
	cmd_size();
	cmd_time();
	arg_short_int[0]=arg_short_int[4];
	cmd_altbias();
	return;
}

void cmd_ldinit()
{
	// Use the initial lattice as the current lattice for subsequent runs.
	memcpy(parameters.spins, parameters.spins_initial, sizeof(short int)*parameters.nx*parameters.ny*parameters.nz);
	return;
}

void cmd_ldlat()
{
	// Use the current lattice as the initial lattice for subsequent runs.
	memcpy(parameters.spins_initial, parameters.spins, sizeof(short int)*parameters.nx*parameters.ny*parameters.nz);
	return;
}

void cmd_randlat()
{
	set_randlat(arg_double[0], &parameters);
	return;
}

void cmd_seed()
{
	set_seed(arg_int[0], &parameters);
	return;
}

void cmd_seed64()
{
	set_seed_64(arg_double[0], arg_double[1], &parameters);
	return;
}

void cmd_settsc()
{
	set_settsc(&parameters);
	return;
}

void cmd_size()
{
	set_size(arg_int[0], arg_int[1], arg_int[2], &parameters);
	return;
}

void cmd_sp()
{
	printf("bias           %hd\n", parameters.bias);
	printf("bc (x,y,z)     %hd %hd %hd\n", parameters.periodic_bc_x, parameters.periodic_bc_y, parameters.periodic_bc_z);
	printf("size (x,y,z)   %d %d %d\n", parameters.nx, parameters.ny, parameters.nz);
	printf("max time       %lf\n", parameters.max_time);
	printf("seed           %d\n", parameters.seed);
	return;
}

void cmd_time()
{
	set_time(arg_double[0], &parameters);
	
	return;
}

