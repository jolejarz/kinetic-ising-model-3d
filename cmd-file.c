/**************************************************************
***************************************************************
*** 
***   cmd-file.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains commands for file I/O.
***
***************************************************************
**************************************************************/

void cmd_load()
{
	short int data_index=arg_short_int[0];
	int size=arg_int[0];
	
	void *output;
	int *output_size;
	
	if (data_output(data, data_parameters, &output, &output_size, size, sizeof(int), data_index, data_num)==1) return;

	FILE *datafile=fopen(arg_string[0], "r");
		
	size=fread(output, 1, size, datafile);

	fclose(datafile);
	
	*output_size=size;
	
	printf("%d bytes were read from %s.\n", size, arg_string[0]);

	return;
}

void cmd_save()
{
	short int data_index=arg_short_int[0];

	void *input;
	int *input_size;
	
	if (data_input(data, data_parameters, &input, &input_size, data_index, data_num)==1) return;

	FILE *datafile=fopen(arg_string[0], "w");
		
	fwrite(input, 1, *input_size, datafile);
	
	fclose(datafile);
	
	printf("%d bytes were written to %s.\n", *input_size, arg_string[0]);
	
	return;
}

