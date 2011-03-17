#ifndef VFS_H
#define VFS_H

#include "../../include/types.h"

typedef struct _VFS_Node VFS_Node;
typedef struct _dirent dirent;

typedef uint32_t (*read_t)(VFS_Node *, uint64_t, uint64_t, uint8_t *);
typedef uint32_t (*write_t)(VFS_Node *, uint64_t, uint64_t, uint8_t *);
typedef void (*open_t)(VFS_Node *);
typedef void (*close_t)(VFS_Node *);
typedef dirent * (*readdir_t)(VFS_Node *, uint64_t);
typedef VFS_Node * (*finddir_t)(VFS_Node *, char *);

uint32_t read_fs(VFS_Node * node, uint64_t offset, uint64_t size, uint8_t * buffer);
uint32_t write_fs(VFS_Node * node, uint64_t offset, uint64_t size, uint8_t * buffer);
void open_fs(VFS_Node * node);
void close_fs(VFS_Node * node);
dirent * readdir_fs(VFS_Node * node, uint64_t index);
VFS_Node * finddir_fs(VFS_Node * node, char * file_path);

extern VFS_Node * vfs_root;

/**
 * Structure defines a directory entity for reading a directory
 *
 * @author Colin Barr
 */
struct _dirent {
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
struct _VFS_Node {
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
	VFS_Node * ptr;
};

#endif