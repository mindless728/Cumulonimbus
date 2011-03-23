#ifndef FAT64_H
#define FAT64_H

#include "../../include/types.h"
#include "../VFS/VFS.h"

#define FAT64_ID 0x7F
#define FAT64_UNIQUE "FT64"

typedef struct _FAT64_directory_entry FAT64_directory_entry;
typedef struct _FAT64_table_entry FAT64_table_entry;
typedef struct _FAT64_config FAT64_config;

//VFS related functions
status_t FAT64_read_fs(VFS_node_t * node, uint64_t offset, uint64_t size, uint8_t * buffer);
status_t FAT64_write_fs(VFS_node_t * node, uint64_t offset, uint64_t size, uint8_t * buffer);
status_t FAT64_open_fs(VFS_node_t * node);
status_t FAT64_close_fs(VFS_node_t * node);
status_t FAT64_readdir_fs(VFS_node_t * node, uint64_t index, VFS_node_t ** ret);
status_t FAT64_finddir_fs(VFS_node_t * node, char * file_path, VFS_node_t ** ret);

//FAT64 specific functions

/**
 * Structures contains the information for an entry into
 *   a directory
 *
 * @author Colin Alexander Barr
 */
struct _FAT64_directory_entry {
	/** the name of the file */
	char file_name[256];
	
	/**  the first cluster id of the file*/
	uint64_t file_cluster;
	
	/** the size of the file in bytes */
	uint64_t file_size;
	
	/** date the file was created */
	uint64_t date_created;
	
	/** date that the file was last modified */
	uint64_t date_modified;
	
	/** date the file was last accessed */
	uint64_t date_accessed;
	
	/** flags for the file
	    bit - description
		  0 - file(0)/directory(1)
	 */
	uint64_t flags;
	
	/** the unused space of the structure */
	uint8_t unused_space[208];
};

/**
 * structure defines an entry into the cluster entry table
 *
 * @author Colin Alexander Barr
 */
struct _FAT64_table_entry {
	/** the previous cluster in the file */
	uint64_t prev;
	
	/** the next cluster in the file */
	uint64_t next;
};

/**
 * Structure contains the information for the configuration
 *   of a FAT64 file system
 *
 * @author Colin Alexander Barr
 */
struct _FAT64_config {
	/** unique id for the file system type */
	char unique[4];
	
	/** size of the volume in bytes */
	uint64_t size;
	
	/** size of the clusters in bytes */
	uint64_t cluster_size;
	
	/** location of the root_directory */
	uint64_t root_directory;
	
	/** location of the cluster table */
	uint64_t cluster_table;
	
	/** initial sector location of volume */
	uint8_t volume_location;
};

#endif