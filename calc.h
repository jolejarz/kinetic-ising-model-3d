/**************************************************************
***************************************************************
*** 
***   calc.h
***
***   Jason Olejarz
***   2010-07-21
***
***   This is the header file for calc.c.
***
***************************************************************
**************************************************************/

void calc(short int (*loop[])(), short int (*step[])(), short int (*save[])(), short int loop_size, short int step_size, short int save_size, short int data_size, void *data[], void *data_parameters[], struct evolve_parameters *parameters);

