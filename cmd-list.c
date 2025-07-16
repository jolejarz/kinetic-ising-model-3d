/**************************************************************
***************************************************************
*** 
***   cmd-list.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains commands for manipulating the 'loop' and 'save' function arrays.
***
***************************************************************
**************************************************************/

void cmd_dl()
{
	short int list=arg_short_int[0];
	short int list_index=arg_short_int[1];
	
	if (list==0 && loop[list_index]!=0) list_delete(loop, list_index, loop_num);
	else if (list==1 && step[list_index]!=0) list_delete(step, list_index, step_num);
	else if (list==2 && save[list_index]!=0) list_delete(save, list_index, save_num);
	
	return;
}

void cmd_sl()
{
	short int list=arg_short_int[0];
	
	if (list==0) list_show(loop, loop_num);
	else if (list==1) list_show(step, step_num);
	else if (list==2) list_show(save, save_num);
	
	return;
}

