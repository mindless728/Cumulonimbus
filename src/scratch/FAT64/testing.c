#include <stdio.h>

#include "FAT64.h"

int main(int argc, char ** argv) {
	printf("FAT64_DE size = %u\n",sizeof(FAT64_Directory_Entry));
	printf("FAT64_Config size = %u\n",sizeof(FAT64_Config));
}