#include <stdio.h>

#include "mbr.h"

int main(int argc, char ** argv) {
	printf("sizeof(MBR_t) = %u\n",sizeof(MBR_t));
	printf("sizeof(MBR_partition_t) = %u\n", sizeof(MBR_partition_t));
	printf("sizeof(CHS_address_t) = %u\n", sizeof(CHS_address_t));
}