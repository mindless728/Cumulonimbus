

void init_phys_alloc(int * start) {

	int page;
	int page_entry; //Page enties are 32 bit

	//It takes 24576 pages to store the physcial memmory allocator
	//Zero out the next 24576 pages
	//pages have 4096 bytes or 1024 32 bit entries

	for(page = 0; page < 24576; page++) {

		start[page] = 0;
	}

	// since the physcial memmory allocater itself takes up 24576 pages,
	// flip the first 24576 bits to allocate the physical memmory for it

	int x;

	for(x = 0; x < (24576 / 32); x++) {
		start[x] = 0xFFFFFFFF;
	}

	return;	
}

void free_phys_page(int * page) {

	// location - page = offset
	// offset / size of page  should be the index of page in the paging area
	// 32 indexes per native memory size should give us how many 32 bit intervals in our memory to free is in

	int delta = ( ( (int)page - (int)phys_mem_table) / (1024 * 4) ) / 32;
	
	int shift = ( ( (int)page - (int)phys_mem_table) / (1024 * 4) ) % 32;

	// clear the bit in the table
	phys_mem_table[delta] &= ~(shift<<0x00000001);

}


int * alloc_phys_page(void) {


	if( phys_mem_table == NULL) {
		//TODO error out because you called this before calling init_phys_alloc
	}


	// find free bit in table
	int x;
	//x does not need to start at the begining, instead it starts searching directly after the permenatnly
	//alocated mem for the table itself
	for(x = (24576 / 32) ; x < 24576; x++) {

		// If this native memory sized block in table is not all taken
		if(phys_mem_table[x] != 0xFFFFFFFF) {
			unsigned int mask;
			int y = 0;
			//From right to left find the first bit that is not clear
			for(mask = 0x00000001; mask != 0x00000000; mask = mask<<1) {
				//if bit is clear

				if((mask & phys_mem_table[x]) == 0) {
					phys_mem_table[x] = phys_mem_table[x] | mask;
					// 32 pages per x
					// 1 page per y
					//start of phys_mem_table + 32 pages per x + y pages
					return (int*)( ( (int)phys_mem_table ) + ( (1024 * 4 * 32 * x ) + ( 1024 * 4 * y ) ) );  
				}
				y++;
			}
		}
	}

	//TODO if we get this far we have run out of free pages, do something

}
