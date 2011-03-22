#include <stdio.h>

#include "VFS.h"

int main(int argc, char ** argv) {
	printf("sizeof(VFS_node_t) = %u\n",sizeof(VFS_node_t));
	printf("sizeof(VFS_functions_t) = %u\n",sizeof(VFS_functions_t));
}