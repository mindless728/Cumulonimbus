#include <stdio.h>

#include "VFS.h"

int main(int argc, char ** argv) {
	printf("sizeof(VFS_Node) = %u\n",sizeof(VFS_Node));
	printf("sizeof(dirent) = %u\n",sizeof(dirent));
}