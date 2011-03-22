#include "VFS.h"

VFS_node_t * vfs_root;
VFS_functions_t vfs_implementation_table[VFS_IMPLEMENTATION_TABLE_SIZE];
uint32_t num_implementations;

status_t read_fs(VFS_node_t * node, uint64_t offset, uint64_t size, uint8_t * buffer) {
	status_t stat = 0;
	if(node->functions != 0 && node->functions->read) {
		stat = node->functions->read(node, offset, size, buffer);
	}
	
	return stat;
}

status_t write_fs(VFS_node_t * node, uint64_t offset, uint64_t size, uint8_t * buffer) {
	status_t stat = 0;
	if(node->functions != 0 && node->functions->write) {
		stat = node->functions->write(node, offset, size, buffer);
	}
	
	return stat;
}

status_t open_fs(VFS_node_t * node) {
	status_t stat = 0;
	if(node->functions != 0  && node->functions->open) {
		stat = node->functions->open(node);
	}
	
	return stat;
}

status_t close_fs(VFS_node_t * node) {
	status_t stat = 0;
	if(node->functions != 0 && node->functions->close) {
		stat = node->functions->close(node);
	}
}

status_t readdir_fs(VFS_node_t * node, uint64_t index, VFS_node_t ** ret) {
	status_t stat = 0;
	if(node->flags && VFS_DIRECTORY != 0 && node->functions != 0 && node->functions->readdir) {
		stat = node->functions->readdir(node, index, ret);
	}
	
	return stat;
}

status_t finddir_fs(VFS_node_t * node, char * file_path, VFS_node_t ** ret) {
	status_t stat = 0;
	if(node->functions != 0 && node->functions->finddir) {
		stat = node->functions->finddir(node, file_path, ret);
	}
	
	return stat;
}

status_t vfs_init() {
	vfs_root = 0;
	num_implementations = 0;
}

status_t vfs_register(char * unique, read_t read, write_t write, open_t open, close_t close, readdir_t readdir, finddir_t finddir) {
	status_t stat = 0;
	VFS_functions_t * f;
	
	if(num_implementations < VFS_IMPLEMENTATION_TABLE_SIZE) {
		f = &(vfs_implementation_table[num_implementations]);
		f->implementation = *((uint32_t *)unique);
		f->read = read;
		f->write = write;
		f->open = open;
		f->close = close;
		f->readdir = readdir;
		f->finddir = finddir;
		stat = 1;
		++num_implementations;
	}
	
	return stat;
}

status_t vfs_mount(char * unique, char * path, VFS_node_t * device) {
}

status_t vfs_umount(char * path) {
}