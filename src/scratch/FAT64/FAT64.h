#ifndef FAT64_H
#define FAT64_H

#include "../../include/types.h"

typedef struct _FAT64_Directory_Entry FAT64_Directory_Entry;
typedef struct _FAT64_Config FAT64_Config;

/**
 * Structures contains the information for an entry into
 *   a directory
 *
 * @author Colin Alexander Barr
 */
struct _FAT64_Directory_Entry {
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
 * Structure contains the information for the configuration
 *   of a FAT64 file system
 *
 * @author Colin Alexander Barr
 */
struct _FAT64_Config {
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