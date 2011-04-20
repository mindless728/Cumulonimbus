#include "FAT64.h"

status_t FAT64_read_fs(VFS_node_t * node, uint64_t offset, uint64_t size, uint8_t * buffer) {
	uint8_t tmp[512];
	uint64_t written = 0;

	while(written != size) {
	}
}

status_t FAT64_write_fs(VFS_node_t * node, uint64_t offset, uint64_t size, uint8_t * buffer) {
	uint8_t tmp[512];
	uint64_t written = 0;

	while(written != size) {
	}
}

status_t FAT64_open_fs(VFS_node_t * node) {
}

status_t FAT64_close_fs(VFS_node_t * node) {
}

status_t FAT64_readdir_fs(VFS_node_t * node, uint64_t index, VFS_node_t ** ret) {
}

status_t FAT64_finddir_fs(VFS_node_t * node, char * file_path, VFS_node_t ** ret) {
}
