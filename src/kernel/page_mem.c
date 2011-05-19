/*
	Name: Christopher Kaczmarek (cmk1412)
	Class: Systems II
	This class handles all dealings with the vertual memory allocator
*/

// Note: protected mem 
// e0000000 to top
// first 512 


//Call to create a page table, give it physcial mem pointer to the table
int * init_table(int * table_start ) {

	int x;
	for(x = 0; x < 1024; x++) {
	
		table_start[x] = 0;
		//pages_start += 4096;
	}

	return table_start;
}
//Call to create a page directory, give it physcial mem pointer to the directory
int * init_page_directory(int * start) {


	int x;
	for(x = 0; x < 1024; x++) {
		start[x] = 0|2;
	}

	return start;	
}

// Call to initilize a page dir, copy to mem, ident map and do everything else that a new process need
// returns an int ptr to the page dir
int * new_process_mem(void){


	int * new_page_dir = alloc_phys_page();
	init_page_directory(new_page_dir);
	

	//We are identity mapping the first and last 512 mb of memory of the 4 gigs oh phys mem
	//what this means is the vertual mem address has a 1:1 corolation with the phys addr
	//This means the first and last 128 indexes in the page directory of 1024 entries

	int x; int y;
	int address = 0;
	for(x = 0; x < 128; x++) {
		int * new_table = (int *)alloc_phys_page();
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
	
	//This copies the executable code into the vert mem space of the process
	//TODO, this thing start copying at 0 to the first 100k of mem, this might break
	int * mem = (int *)0;
	for(x = 0; x < 25 ; x++ ) {
		
		for(y = 0; y < 1024 ; y+=4) {
			//TODO turn paging on
			alloc_vur_page((63 << 22)|(x<<12)|y);
			((int*)((63 << 22)|(x<<12)|y))[0] = mem[0];
			//TODO turn paging off
			mem++;
		}
	}

	//Allocate stack
	alloc_vur_page(600<<22);

	return new_page_dir;

}

// Call to translate a linear address to a physcial one
// returns physcial addr
int * get_phys_address(void * vert_address) {

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

	unsigned int cr3;

	asm volatile( "movl %%cr3, %0\n\t":"=r"(cr3));

	unsigned int page_dir_entry = ((unsigned int *) cr3)[page_dir_offset];

	if( (page_dir_entry & 0xFFFFF000) == 0 || (page_dir_entry & 0x00000001) == 0) {
		//TODO do something!
		return 0;
	}

	//If you get this far you can strip the status bits off the entry
	page_dir_entry &= 0xFFFFF000;

	unsigned int phys_mem_address = ((unsigned int *) page_dir_entry)[page_table_offset];

	if( (phys_mem_address & 0xFFFFF000) == 0 || (phys_mem_address & 0x00000001) == 0) {
		//TODO do something
		return 0;
	}

	return (unsigned int) ( ((int)phys_mem_address & 0xFFFFF000 ) | ((int)vert_address & 0x00000FFF) );
 	
}


// Call to allocate a page, give the desired vurtual address, flags is not implented (yet)
void alloc_vur_page( int * vert_address, unsigned int flags) {

	unsigned long page_dir_offset = (unsigned long)vert_address >> 22;	  
	unsigned long page_table_offset = ((unsigned long)vert_address >> 12) & 0x000003FF;

	// Grab cr3 (page dir adder)
	unsigned int cr3;
	asm volatile( "movl %%cr3, %0\n\t":"=r"(cr3));

	unsigned int page_dir_entry = ((unsigned int *)cr3)[page_dir_offset];

	unsigned int * new_table;

	// This will clober a pre-exsist page, don't double allocate mem!
	if( page_dir_entry == 0 ) {

		new_table = alloc_phys_page();
		init_table( new_table);
	} else {
		new_table = page_dir_entry;
	}

	unsigned int table_entry = new_table[page_table_offset];
	int * page_allocated;

	page_allocated = alloc_phys_page();
	new_table[page_table_offset] = (int)page_allocated | 3;


	// please don't alocate an already allocated page, 
	// the old page may never be freed

	__asm__ volatile("invlpg %0"::"m" (*(char *)vert_address));
	
}



