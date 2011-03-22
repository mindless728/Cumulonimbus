#ifndef VFS_H
#define VFS_H

#include "../../include/types.h"

#define VFS_FILE 0x1
#define VFS_DIRECTORY 0x2
#define VFS_CHARDEVICE 0x3
#define VFS_BLOCKDEVICE 0x4
#define VFS_PIPE 0x5
#define VFS_SYMLINK 0x6
#define VFS_MOUNTPOINT 0x8

#define VFS_IMPLEMENTATION_TABLE_SIZE 16

typedef struct _VFS_functions_t VFS_functions_t;
typedef struct _VFS_node_t VFS_node_t;

typedef status_t (*read_t)(VFS_node_t *, uint64_t, uint64_t, uint8_t *);
typedef status_t (*write_t)(VFS_node_t *, uint64_t, uint64_t, uint8_t *);
typedef status_t (*open_t)(VFS_node_t *);
typedef status_t (*close_t)(VFS_node_t *);
typedef status_t (*readdir_t)(VFS_node_t *, uint64_t, VFS_node_t **);
typedef status_t (*finddir_t)(VFS_node_t *, char *, VFS_node_t **);

status_t read_fs(VFS_node_t * node, uint64_t offset, uint64_t size, uint8_t * buffer);
status_t write_fs(VFS_node_t * node, uint64_t offset, uint64_t size, uint8_t * buffer);
status_t open_fs(VFS_node_t * node);
status_t close_fs(VFS_node_t * node);
status_t readdir_fs(VFS_node_t * node, uint64_t index, VFS_node_t ** ret);
status_t finddir_fs(VFS_node_t * node, char * file_path, VFS_node_t ** ret);

status_t vfs_init();
status_t vfs_register(char * unique, read_t read, write_t write, open_t open, close_t close, readdir_t readdir, finddir_t finddir);
status_t vfs_mount(char * unique, char * path, VFS_node_t * device);
status_t vfs_umount(char * path);

extern VFS_node_t * vfs_root;

struct _VFS_functions_t {
	uint32_t implementation;
	read_t read;
	write_t write;
	open_t open;
	close_t close;
	readdir_t readdir;
	finddir_t finddir;
};

struct _VFS_node_t {
	char file_name[256];
	uint64_t inode;
	uint64_t length;
	uint32_t flags;
	VFS_node_t * link;
	VFS_functions_t * functions;
};

#endif