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

typedef struct _VFS_node_t VFS_node_t;
typedef struct _dirent_t dirent_t;

typedef status_t (*read_t)(VFS_node_t *, uint64_t, uint64_t, uint8_t *);
typedef status_t (*write_t)(VFS_node_t *, uint64_t, uint64_t, uint8_t *);
typedef void (*open_t)(VFS_node_t *);
typedef void (*close_t)(VFS_node_t *);
typedef dirent_t * (*readdir_t)(VFS_node_t *, uint64_t);
typedef VFS_node_t * (*finddir_t)(VFS_node_t *, char *);

status_t read_fs(VFS_node_t * node, uint64_t offset, uint64_t size, uint8_t * buffer);
status_t write_fs(VFS_node_t * node, uint64_t offset, uint64_t size, uint8_t * buffer);
void open_fs(VFS_node_t * node);
void close_fs(VFS_node_t * node);
dirent_t * readdir_fs(VFS_node_t * node, uint64_t index);
VFS_node_t * finddir_fs(VFS_node_t * node, char * file_path);

extern VFS_node_t * vfs_root;

/**
 * Structure defines a directory entity for reading a directory
 *
 * @author Colin Barr
 */
struct _dirent_t {
	/** the name of the file */
	char file_name[256];
	
	/** the file's inode number */
	uint64_t inode;
};

/**
 * Structure defines how filesytems should talk to the VFA system
 *
 * @author Colin Barr
 */
struct _VFS_node_t {
	/** the name of the file on the FS */
	char file_name[256];
	
	/** the inode number of the file */
	uint64_t inode;
	
	/** the length of the file */
	uint64_t length;
	
	/** flags for the node type */
	uint32_t flags;
	
	/** permissions mask */
	uint32_t mask;
	
	/** user id */
	uint32_t uid;
	
	/** group id */
	uint32_t gid;
	
	/** immplementation identification */
	uint32_t impl;
	
	/** used by mountpoints and symlinks */
	VFS_node_t * ptr;
	
	/** how the underlying FS reads */
	read_t read;
	
	/** how the underlying FS writes */
	write_t write;
	
	/** how the underlying FS opens */
	open_t open;
	
	/** how the underlying FS closes */
	close_t close;
	
	/** how the underlying FS reads dirs */
	readdir_t readdir;
	
	/** how the underlying FS finds dirs */
	finddir_t finddir;
};

#endif