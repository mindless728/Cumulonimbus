#ifndef FAT64_H
#define FAT64_H

#include <types.h>
#include <drivers/ide/ide.h>
#include <include/mbr.h>

#include "fat64_defines.h"

typedef struct _fat64_partition_t fat64_partition_t;
typedef struct _fat64_cluster_tags_t fat64_cluster_tags_t;
typedef struct _fat64_dir_entry_t fat64_dir_entry_t;
typedef struct _fat64_file_t fat64_file_t;
typedef struct _fat64_file_t FILE;

struct _fat64_partition_t {
	uint64_t root_dir; //location of the root directory cluster
	uint64_t cluster_tags; //location of the cluster tag table starting cluster
	uint64_t size; //size of the partition in clusters
	uint64_t cluster_size; //size of a cluster, always 4k
	uint8_t reserver[480];
};

struct _fat64_cluster_tags_t {
	uint64_t prev; //previous cluster in the file
	uint64_t next; //next cluster in the file
};

struct _fat64_dir_entry_t {
	char name[256]; //file name

	uint64_t file; //cluster for file
	uint64_t parent; //cluster of parent directory
	uint64_t size; //size of the file in bytes

	uint64_t time_created; //time file was created
	uint64_t time_accessed; //time file was last accessed
	uint64_t time_modified; //time file was last modified

	uint32_t flags; //file flags
	uint32_t dir_entry_num; //directory entry number

	uint8_t unused[200]; //unused space
};

struct _fat64_file_t {
	fat64_dir_entry_t entry; //directory entry for the file
	fat64_cluster_tags_t current_tags; //tags for the current location
	uint64_t entry_num; //the entry into the directory the file is
	uint64_t cluster; //current cluster location in file
	uint64_t abs_location; //the current location of the file
	uint32_t location; //location into current cluster
	uint8_t buf[4096]; //buffer for the current cluster
	boolean_t dirty;
};

//kernel functions
status_t _fat64_init(void);
status_t _fat64_mkfs(uint64_t size, mbr_t * mbr);
static void _fat64_user_init(context_t * context); //fills in the partition info for the user

//generic functions
status_t fat64_user_init(void);
status_t fat64_open(handle_t file, char * path);
status_t fat64_close(handle_t file);
status_t fat64_is_directory(handle_t file);

//file functions
status_t fat64_rm(handle_t file);
status_t fat64_mv(handle_t file, handle_t dir);
status_t fat64_getc(handle_t file, uint8_t * data);
status_t fat64_putc(handle_t file, uint8_t data);
status_t fat64_read(handle_t file, uint64_t amount, uint8_t * buf);
status_t fat64_write(handle_t file, uint64_t amount, uint8_t * buf);
status_t fat64_seek(handle_t file, int64_t offset, uint8_t type);
status_t fat64_tell(handle_t file, uint64_t * offset);
status_t fat64_flush(handle_t file);

//directory functions
status_t fat64_dir_entry(handle_t dir, uint64_t index, handle_t file);
status_t fat64_mkdir(handle_t dir, char * name);
status_t fat64_touch(handle_t dir, char * name);
status_t fat64_rmdir(handle_t dir);

#endif
