void init_paging(void) {

	//TODO make this line into a define
	phys_mem_table = (int *)1073741824;  //This should be the start of the 2nd gig in ram
//	phys_mem_table = (int*) malloc((24576 * sizeof(int*)) + 3);

	if(phys_mem_table == NULL) {
		printf("Failed to malloc phys_mem_table\n");
	}

	int offset = ((int)phys_mem_table) % 4;
	phys_mem_table += offset;
	
	init_phys_alloc(phys_mem_table);

}

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
	unsigned long page_table_offset = ((unsigned long)vert_address >> 12) & 0x00000FFF;

	uint32_t var;

	asm volitie( "movl %%cr3, %0\n\t":"=r"(var));

	uint32_t page_dir_entry = ((uint32_t *) var)[page_dir_offset];
	page_dir_entry = (page_dir_entry >> 12) << 12;

	if( page_dir_entry & ~(0x00000FFF) == NULL || page_dir_entry & 0x00000001 == 0) {
		//TODO do something!
		return NULL;
	}

	uint32_t phys_mem_address = (uint32_t * page_dir_entry)[page_table_offset];

	if(phys_mem_address & ~(0x00000FFF) == NULL || phys_mem_address & 0x00000001 == 0) {
		//TODO do something
		return NULL;
	}

	return (uint32_t) ( (phys_mem_address & ~(0x00000FFF) ) | (vert_address & 0x00000FFF) );
 	
}



int * init_page_table(int * table_start, int * pages_start ) {

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

/* Old attempt */

/*
	int next_page =  start;
	int * page_dir =  start + (24576 / 32);
	int x;	
	int page_table_address = (int*) ( (int)page_dir + 4096 );
	int address = (int) page_table_address;
	for(x = 0; x < 768; x++) {

		page_dir[x] = address | 3; 
		address += 4096;

		int y;

		for(y = 0; y < 1024; y++) {
		
			((int*)address)[y] = next_page | 3;
			next_page += 4096;
		}
	}
	
*/	
}
