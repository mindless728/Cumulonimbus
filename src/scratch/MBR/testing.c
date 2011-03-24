#include <stdio.h>

#include "mbr.h"

int main(int argc, char ** argv) {
	printf("sizeof(MBR_t) = %lu\n",sizeof(MBR_t));
	printf("sizeof(MBR_partition_t) = %lu\n", sizeof(MBR_partition_t));
	printf("sizeof(CHS_address_t) = %lu\n", sizeof(CHS_address_t));
}