#include <stdio.h>

#include "VFS.h"

int main(int argc, char ** argv) {
	printf("sizeof(VFS_Node) = %u\n",sizeof(VFS_node_t));
	printf("sizeof(dirent) = %u\n",sizeof(dirent_t));
}