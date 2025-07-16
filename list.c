/**************************************************************
***************************************************************
*** 
***   list.c
***
***   Jason Olejarz
***   2010-07-21
***
***   This file contains the routine used to manage the 'load' and 'save' function pointer arrays.
***
***************************************************************
**************************************************************/

#include "includes.h"

void list_delete(void (*list[])(), short int list_index, short int list_size)
{
	if (list[list_index]!=0 && list_index<list_size)
	{
		list[list_index](command_delete, 0, 0, 0, 0, 0);
		list[list_index]=0;
	
		while (list[++list_index]!=0)
		{
			list[list_index-1]=list[list_index];
			list[list_index]=0;
		}
	}
	
	return;
}

void list_insert(void (*list[])(), short int list_index, void *list_function, short int list_size)
{
	int i=0;
	
	if (list_index>=list_size)
	{
		printf("The list index must be from 0 to %hd.\n", list_size-1);
		return;
	}
	
	while (list[i]!=0) i++;

	if (i>list_index)
	{
		for (; i>list_index; i--)
		{
			list[i]=list[i-1];
		}
	}
	else list_index=i;
	
	list[list_index]=list_function;
	
	return;
}

void list_show(void (*list[])(), short int list_size)
{
	int i=0;

	while (list[i]!=0 && i<list_size)
	{
		list[i](command_info, 0, 0, 0, 0, 0);
		i++;
	}

	return;
}

