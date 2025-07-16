/**************************************************************
***************************************************************
*** 
***   list.h
***
***   Jason Olejarz
***   2010-07-21
***
***   This is the header file for list.c.
***
***************************************************************
**************************************************************/

enum {command_begin, command_delete, command_end, command_info, command_insert, command_run};
enum {result_continue, result_done, result_error, result_skip};

void list_delete(void (*list[])(), short int list_index, short int list_size);
void list_insert(void (*list[])(), short int list_index, void *list_function, short int list_size);
void list_show(void (*list[])(), short int list_size);

