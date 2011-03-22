

void enable_paging(void) {
	__asm__("mov	%cr3,	POINTER_TO_PAGE_TABLE\n\t" 
		"or 	%cr0,	0x80000000 \n\t"); //Set paging bit
	}


