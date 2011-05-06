#ifndef FAT64_H
#define FAT64_H

#include <types.h>
#include <drivers/ide/ide.h>

#include "fat64_defines.h"

typedef struct _fat64_cluster_tags_t fat64_cluster_tags_t;
typedef struct _fat64_dir_entry_t fat64_dir_entry_t;
typedef struct _fat64_file_t fat64_file_t;
typedef struct _fat64_file_t FILE;

struct fat64_partition {
	uint64_t root_dir; //location of the root directory
	uint64_t cluster_tags; //location of the cluster tag table
	uint64_t size; //size of the partition in clusters
	uint64_t cluster_size; //size of a cluster, always 4k
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

	uint8_t unused[204]; //unused space
};

struct _fat64_file_t {
	fat64_dir_entry_t entry; //directory entry for the file
	fat64_cluster_tags_t current_tags; //tags for the current location
	uint64_t location; //the current location of the file
	uint8_t buf[4096]; //buffer for the current cluster
	boolean_t dirty;
};

//generic functions
status_t _fat64_init(void);
status_t open(handle_t * file, char * path);
status_t close(handle_t file);
status_t is_directory(handle_t file);

//file functions
status_t rm(handle_t file);
status_t mv(handle_t file, handle_t dir);
status_t getc(handle_t file, uint8_t * data);
status_t putc(handle_t file, uint8_t data);
status_t read(handle_t file, uint64_t amount, uint8_t * buf);
status_t write(handle_t file, uint64_t amount, uint8_t * buf);
status_t seek(handle_t file, int64_t offset, uint8_t type);
status_t tell(handle_t file, uint64_t * offset);
status_t flush(handle_t file);

//directory functions
status_t dir_entry(handle_t dir, uint64_t index, handle_t * file);
status_t mkdir(handle_t dir, char * name);
status_t touch(handle_t dir, char * name);
status_t rmdir(handle_t dir);

#endif
