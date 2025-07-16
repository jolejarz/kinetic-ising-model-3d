/**************************************************************
***************************************************************
*** 
***   cmd-loop.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains commands for setting up preprocessing.
***
***************************************************************
**************************************************************/

void cmd_lp()
{
	short int loop_index=arg_short_int[0];
	int count=arg_int[0];
	
	struct loop_iterate_data iteratedata;
	
	iteratedata.count=count;
	loop_iterate(command_insert, &iteratedata, data, data_parameters, data_num, &parameters);

	list_insert(loop, loop_index, loop_iterate, loop_num);
	
	return;
}

void cmd_lpseed()
{
	short int loop_index=arg_short_int[0];
	short int data_index=arg_short_int[1];
	int num=arg_int[0];
	
	struct loop_seed_data seeddata;
	
	seeddata.data_index=data_index;
	seeddata.num=num;
	loop_seed(command_insert, &seeddata, data, data_parameters, data_num, &parameters);

	list_insert(loop, loop_index, loop_seed, loop_num);
	
	return;
}

void cmd_lpseed64()
{
	short int loop_index=arg_short_int[0];
	short int data_index_low=arg_short_int[1];
	short int data_index_high=arg_short_int[2];
	int num=arg_int[0];
	
	struct loop_seed_64_data seed64data;
	
	seed64data.data_index_low=data_index_low;
	seed64data.data_index_high=data_index_high;
	seed64data.num=num;
	loop_seed_64(command_insert, &seed64data, data, data_parameters, data_num, &parameters);

	list_insert(loop, loop_index, loop_seed_64, loop_num);
	
	return;
}

void cmd_randinit()
{
	short int loop_index=arg_short_int[0];
	double init_mag=arg_double[0];
	
	struct loop_randinit_data randinitdata;
	
	randinitdata.init_mag=init_mag;
	loop_randinit(command_insert, &randinitdata, data, data_parameters, data_num, &parameters);

	list_insert(loop, loop_index, loop_randinit, loop_num);
	
	return;
}

void cmd_sizelin()
{
	short int loop_index=arg_short_int[0];
	int min=arg_int[0];
	int increment=arg_int[1];
	int iterations=arg_int[2];
	
	struct loop_size_linear_data sizelineardata;
	
	sizelineardata.min=min;
	sizelineardata.increment=increment;
	sizelineardata.iterations=iterations;
	
	loop_size_linear(command_insert, &sizelineardata, data, data_parameters, data_num, &parameters);

	list_insert(loop, loop_index, loop_size_linear, loop_num);
	
	return;
}

void cmd_tsc()
{
	short int loop_index=arg_short_int[0];

	list_insert(loop, loop_index, loop_seed_tsc, loop_num);

	return;
}

void cmd_tsc64()
{
	short int loop_index=arg_short_int[0];

	list_insert(loop, loop_index, loop_seed_tsc_64, loop_num);

	return;
}

