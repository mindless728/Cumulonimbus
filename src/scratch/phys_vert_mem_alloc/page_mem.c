

uint32_t * get_phys_address(void * vert_address) {

/*
US RW  P - Description
0  0  0 - Supervisory process tried to read a non-present page entry
0  0  1 - Supervisory process tried to read a page and caused a protection fault
0  1  0 - Supervisory process tried to write to a non-present page entry
0  1  1 - Supervisory process tried to write a page and caused a protection fault
1  0  0 - User process tried to read a non-present page entry
1  0  1 - User process tried to read a page and caused a protection fault
1  1  0 - User process tried to write to a non-present page entry
1  1  1 - User process tried to write a page and caused a protection fault
*/

	unsigned long page_dir_offset = (unsigned long)vert_address >> 22;	  
	unsigned long page_table_offset = ((unsigned long)vert_address >> 12) & 0x000003FF;

	uint32_t cr3;

	asm volatile( "movl %%cr3, %0\n\t":"=r"(cr3));

	uint32_t page_dir_entry = ((uint32_t *) cr3)[page_dir_offset];

	if( page_dir_entry & 0xFFFFF000 == NULL || page_dir_entry & 0x00000001 == 0) {
		//TODO do something!
		return NULL;
	}

	//If you get this far you can strip the status bits off the entry
	page_dir_entry &= 0xFFFFF000;

	uint32_t phys_mem_address = ((uint32_t *) page_dir_entry)[page_table_offset];

	if(phys_mem_address & 0xFFFFF000 == NULL || phys_mem_address & 0x00000001 == 0) {
		//TODO do something
		return NULL;
	}

	return (uint32_t) ( (phys_mem_address & 0xFFFFF000 ) | (vert_address & 0x00000FFF) );
 	
}

void alloc_vur_page(int * physaddr, void * virtualaddr, unsigned int flags) {

	//TODO finish this function, its important
	
}



int init_table(int * table_start, int * pages_start ) {

	int x;
	for(x = 0; x < 1024; x++) {
	
		table_start[x] = pages_start | 3;
		pages_start += 4096;
	]

	return table_start;
}

int * init_page_directory(int * start) {


	int x;
	for(x = 0; x < 1024; x++) {
		start[x] = 0|2;
	}

	return start;	
}
