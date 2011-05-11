// e0000000 to top
// first 512 mb of bottom

int * new_process_mem(void){


	int * new_page_dir = alloc_phys_page();
	init_page_directory(new_page_dir);
	
	//TODO Flush entry from tlb, since its a new directory you may not have to do this

	//We are identity mapping the first and last 512 mb of memory of the 4 gigs oh phys mem
	//what this means is the vertual mem address has a 1:1 corolation with the phys addr
	//This means the first and last 128 indexes in the page directory of 1024 entries

	int x; int y;
	int address = 0;
	for(x = 0; x < 128; x++) {
		int * new_table = alloc_phys_page();
		init_table(new_table);
		
		for(y = 0; y < 1024; y++) {
			new_table[y] = address | 5;
			address += 4096;
		}

		new_page_dir[x] = new_table;		
	}

	for(x = 0; x < 128; x++) {
		int * new_table = alloc_phys_page();
		init_table(new_table);
		
		for(y = 0; y < 1024; y++) {
			new_table[y] = address | 5;
			address += 4096;
		}

		new_page_dir[x + 896] = new_table;
	}


	return new_page_dir;

}

// we don't ever get to call this function
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

void alloc_vur_page( uint32_t * vert_address, unsigned int flags) {

	unsigned long page_dir_offset = (unsigned long)vert_address >> 22;	  
	unsigned long page_table_offset = ((unsigned long)vert_address >> 12) & 0x000003FF;

	uint32_t cr3;
	asm volatile( "movl %%cr3, %0\n\t":"=r"(cr3));

	uint32_t page_dir_entry = (uint32_t *)cr3[page_dir_offset];

	uint32_t * new_table;

	if( page_dir_entry page_dir_entry == NULL ) {

		table = alloc_phys_page();
		init_table( new_table);
	} else {
		table = page_dir_entry;
	}

	uint32_t table_entry = table[page_table_offset];

	if (table_entry == NULL) {
		//TODO, this should not be null, can you alloc over an already exsisting page?
	} else {
		table[page_table_offset] = alloc_phys_page() | 3;
	}

	//TODO finish this function, its important
	
}



uint32_t * init_table(int * table_start ) {

	int x;
	for(x = 0; x < 1024; x++) {
	
		table_start[x] = 0;
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
