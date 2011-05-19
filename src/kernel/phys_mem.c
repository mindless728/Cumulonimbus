/*
	Name: Christopher Kaczmarek (cmk1412)
	Class: Systems II
	This file handles any and all requests, frees and initilization
	of the physcial mem allocator
*/


// Protected mem
// e0600000 to top
// first 512 of bottom

//Address of the physical memmory table, its at the end of the 2nd gig
int * phys_mem_table = (int *) 2147483648;


// Called by phy_vert_mem_init.c in the process of creating a new page
// don't call this unless you are phy_vert_mem_init.c
void init_phys_alloc(void) {

	int * phys_mem_table = (int *)1073741824;  //This should be the start of the 1st gig in ram


	int page;
//	int page_entry; //Page enties are 32 bit

	//It takes 24576 pages to store the physcial memmory allocator
	// 3 gigabytes / 4 kilobytes / 32
	// memory for 3 gigs in 4 kilobyte chunks stored 32 to each uint32
	//Zero out the next 24576 pages
	//pages have 4096 bytes or 1024 32 bit entries

	for(page = 0; page < 20480; page++) {

		phys_mem_table[page] = 0;
	}

	// since the physcial memmory allocater itself takes up 24576 pages,
	// flip the first 24576 bits to allocate the physical memmory for it

	int x;

	for(x = 0; x < (20480 / 32); x++) {
		phys_mem_table[x] = 0xFFFFFFFF;
	}

	return;
}

// Use to free a phys page from mem
void free_phys_page(int * linear_address) {

	unsigned int phys_addr = get_phys_address((void *) linear_address);

	// location - page = offset
	// offset / size of page  should be the index of page in the paging area
	// 32 indexes per native memory size should give us how many 32 bit intervals in our memory to free is in

	int delta = ( ( (int)phys_addr - (int)phys_mem_table ) / (1024 * 4) ) / 32;
	
	int shift = ( ( (int)phys_addr - (int)phys_mem_table ) / (1024 * 4) ) % 32;

	// clear the bit in the table
	phys_mem_table[delta] &= ~(shift<<0x00000001);

	//Remove page from tlb
	//TODO make sure this works
	__asm__ volatile("invlpg %0"::"m" (*(char *)linear_address));

}


// Call to allocate a page
// returns the physcial address to the 4k alligned page
int * alloc_phys_page(void) {


	if( phys_mem_table == 0) {

		return 0;
		// Error out because you called this before calling init_phys_alloc
		// thisshould not happen
	}


	// find free bit in table
	int x;
	//x does not need to start at the begining, instead it starts searching directly after the permenatnly
	//alocated mem for the table itself as this while never move
	for(x = (24576 / 32) ; x < 20480; x++) {

		// If this native memory sized block in table is not all taken
		if(phys_mem_table[x] != 0xFFFFFFFF) {
			unsigned int mask;
			int y = 0;
			//From right to left find the first bit that is not clear
			for(mask = 0x00000001; mask != 0x00000000; mask = mask<<1) {

				if((mask & phys_mem_table[x]) == 0) {
					phys_mem_table[x] = phys_mem_table[x] | mask;
					// 32 pages per x
					// 1 page per y
					//start of phys_mem_table + 32 pages per x + y pages
					int * linear_address = (int*)( ( (int)phys_mem_table ) + ( (1024 * 4 * 32 * x ) + ( 1024 * 4 * y ) ) );
					//TODO make sure this works
					__asm__ volatile("invlpg %0"::"m" (*(char *)linear_address));
					return linear_address ;  
				}
				y++;
			}
		}
	}

	//TODO If we get this far we have run out of free pages, do something

}
