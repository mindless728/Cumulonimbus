#include "VFS.h"

VFS_node_t * vfs_root;

status_t read_fs(VFS_node_t * node, uint64_t offset, uint64_t size, uint8_t * buffer) {
	if(node->read != 0)
		return node->read(node, offset, size, buffer);
	return 0;
}

status_t write_fs(VFS_node_t * node, uint64_t offset, uint64_t size, uint8_t * buffer) {
	if(node->write != 0)
		return node->write(node, offset, size, buffer);
	return 0;
}

void open_fs(VFS_node_t * node) {
	if(node->open != 0)
		node->open(node);
}

void close_fs(VFS_node_t * node) {
	if(node->close != 0)
		node->close(node);
}

dirent_t * readdir_fs(VFS_node_t * node, uint64_t index) {
	if((node->flags & 0x7) == VFS_DIRECTORY && node->readdir != 0)
		return node->readdir(node, index);
}

VFS_node_t * finddir_fs(VFS_node_t * node, char * file_path) {
	if(node->finddir != 0)
		return node->finddir(node, file_path);
}