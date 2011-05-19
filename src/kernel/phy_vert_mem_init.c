/*
	Name: Christopher Kaczmarek (cmk1412)
	Class: Systems II
	This handles initilzation of paging, don't make
	calls to page_mem and phys_mem yourself
*/

// Call this every time you need a new vertual mem space
// (this used to have more things in it and it remains
//  hear incase we need to add more steps
int * init_process_pagemem(void){

	return (int *) new_process_mem();

	//init_paging();
}


//Call in system before enabling interuts
void init_paging(void) {

	unsigned int cr3;
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

	init_phys_alloc();
}
