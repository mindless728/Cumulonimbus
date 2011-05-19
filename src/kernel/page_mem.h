

#ifndef _PAGE_MEM_H
#define _PAGE_MEM_H

#include "phys_mem.h"

int * init_table(int * table_start );
int * init_page_directory(int * start);
int * new_process_mem(void);
int * get_phys_address(void * vert_address);
void alloc_vur_page( int * vert_address, unsigned int flags);





#endif 
