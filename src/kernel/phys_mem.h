

#ifndef _PHYS_MEM_H
#define _PHYS_MEM_H

#include "page_mem.h"

void init_phys_alloc(void);
void free_phys_page(int * linear_address);
int * init_table(int * table_start );
int * init_page_directory(int * start);
int * alloc_phys_page(void);





#endif 
