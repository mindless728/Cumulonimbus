

void main(void){

	init_phys_alloc();



	init_paging();
}


void init_paging(void) {

	uint32_t cr3;
	asm volatile( "movl %%cr3, %0\n\t":"=r"(cr3));

	//What page directory do we load in?
	//we only care about what page dir is after switching contexts, so put in nothing I guess
	//asm volatile("mov %0, %%cr3":: "b"(c));
	
	//Holds cr0 reg
	unsigned int cr0;
	//Pullout cr0 and flip page enable bit on
	asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= (0x80000000);
	asm volatile("mov %0, %%cr0":: "b"(cr0));

	//Paging is now enabled
}
