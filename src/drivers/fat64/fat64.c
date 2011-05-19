#include "fat64.h"
#include <kernel/system.h>
#include <kernel/syscall.h>
#include <kernel/user.h>

#define FAT64_FS_SIZE 0x40000000
//#define MKFS_DEBUG
//#define FAT64_DEBUG_MSG

ide_device_t * ide_device;
fat64_partition_t fat64_partition;

//kernel helper functions
void _mark_cluster(uint64_t cluster, uint64_t prev, uint64_t next);

//user space helper functions
void open_root(fat64_file_t * f);
void load_cluster(fat64_file_t * f);
void save_cluster(fat64_file_t * f);
void open_cluster(fat64_file_t * f);
void load_dir_entry(fat64_file_t * f);
void save_dir_entry(fat64_file_t * f);
void load_cluster_tags(fat64_file_t * file);
void save_cluster_tags(fat64_file_t * f);
uint64_t get_free_cluster(uint64_t prev, uint64_t next);
uint64_t get_free_dir_entry(fat64_file_t * d);
void mark_cluster(uint64_t cluster, uint64_t prev, uint64_t next);
//uint64_t get_free_cluster(void);
//void mark_cluster(uint64_t cluster, uint64_t prev, uint64_t next);
//void get_cluster_tag(uint64_t cluster, fat64_cluster_tags_t * tag);
//void move_to_next_cluster(fat64_file_t * f);
//void move_to_prev_cluster(fat64_file_t * f);
//void load_cluster(int64_t cluster, uint8_t * buf);
//void save_cluster(int64_t cluster, uint8_t * buf);
//void update_dir_entry(fat64_file_t * f);
//uint64_t get_free_dir_entry(fat64_file_t * d);

//note does not fill in f->entry information!!!
//void open_by_cluster(fat64_file_t * f, uint64_t cluster);

//kernel functions
status_t _fat64_init(void) {
	ide_device = &(ide_devices[1]);
	mbr_t mbr;

	//check for existing FS on disk
	ide_pio_lba_read(ide_device,0,(uint8_t*)&mbr);
	if(((mbr.partition[0].status != 0x00) && (mbr.partition[0].status != 0x80)) || (mbr.MBR_signature != MBR_SIG)) {
		//if it doesn't create it
		_fat64_mkfs(FAT64_FS_SIZE, &mbr);
	} else {
		c_printf("FAT64 Filesystem Found!\n");
	}

	if(mbr.partition[0].partition_type != FAT64_PARTITION_TYPE) {
		c_printf("***ERROR*** FAT64 Filesystem not found - wroing type in MBR");
		return E_ERROR;
	}

	//read in FS specific partition info
	memset(&fat64_partition,0,sizeof(fat64_partition));
	ide_pio_lba_read(ide_device, mbr.partition[0].first_LBA, (uint8_t*)&fat64_partition);
#ifdef FAT64_DEBUG_MSG
	c_printf("Size in Clusters: 0x%x\n", fat64_partition.size);
	c_printf("Root Directory Cluster: 0x%x\n",fat64_partition.root_dir);
	c_printf("Cluster Tag Location Cluster: 0x%x\n",fat64_partition.cluster_tags);
	c_printf("Partition Cluster Size: 0x%x\n",fat64_partition.size);
	uint32_t i;for(i=0;i<0x10000000;++i);
#endif

	//register system call with sys-call system
	if(_syscall_install(_fat64_user_init, FAT64_SYS_CALL_NUMBER)) {
		c_printf("***ERROR*** FAT64 System could not install syscall");
		return E_ERROR;
	}

	c_printf("FAT64 Filesystem Initialized\n");

	return E_SUCCESS;
}

status_t _fat64_mkfs(uint64_t size, mbr_t * mbr) {
	uint32_t start_lba = 8;
	uint32_t number_sectors = size/SECTOR_SIZE;
	uint32_t i = 0;
	//size of cluster tag table in clusters
	uint64_t cluster_tag_size = 16*((number_sectors*SECTOR_SIZE)/(FAT64_CLUSTER_SIZE*FAT64_CLUSTER_SIZE));
	uint32_t current_sector = 0;
	uint8_t buf[512] = {0};
	uint32_t prev,next;

	c_printf("Creating FAT64 Partition\n");

	//initialize an MBR
	memset((void*)mbr,0,sizeof(mbr_t));

	mbr->partition[0].first_LBA = start_lba;
	mbr->partition[0].num_sectors = number_sectors;
	mbr->partition[0].partition_type = FAT64_PARTITION_TYPE;
	mbr->partition[0].status = 0x00;
	mbr->MBR_signature = MBR_SIG;

	ide_pio_lba_write(ide_device,0,(uint8_t*)mbr);	

	//initialize the FS super block
	memset(&fat64_partition,0,sizeof(fat64_partition_t));

	fat64_partition.partition_location = start_lba;
	fat64_partition.cluster_tags = 1;
	fat64_partition.root_dir = cluster_tag_size+fat64_partition.cluster_tags;
	fat64_partition.size = 262144/*(mbr->partition[0].num_sectors/FAT64_CLUSTER_SIZE)*SECTOR_SIZE*/;
	fat64_partition.cluster_size = FAT64_CLUSTER_SIZE;
	
	ide_pio_lba_write(ide_device,start_lba,(uint8_t*)&fat64_partition);

	//mark the superblock cluster as used
	_mark_cluster(0,FAT64_CLUSTER_TAG_EOF,FAT64_CLUSTER_TAG_EOF);

	//create cluster tag table
	for(i = 0; i < cluster_tag_size; ++i) {
		//mark cluster as allocated with prev and next set
		prev = i;
		if(i == 0)
			prev = FAT64_CLUSTER_TAG_EOF;
		next = i + 2;
		if(i == (cluster_tag_size - 1))
			next = FAT64_CLUSTER_TAG_EOF;
		_mark_cluster(i+fat64_partition.cluster_tags,prev,next);
	}

	//create the root directory table
	_mark_cluster(fat64_partition.root_dir,FAT64_CLUSTER_TAG_EOF,FAT64_CLUSTER_TAG_EOF);
}

void _mark_cluster(uint64_t cluster, uint64_t prev, uint64_t next) {
	uint32_t cluster_tags_per_sector = SECTOR_SIZE/sizeof(fat64_cluster_tags_t);
	uint32_t sectors_per_cluster = FAT64_CLUSTER_SIZE/SECTOR_SIZE;
	uint32_t sector_to_use = ((uint32_t)cluster)/cluster_tags_per_sector+sectors_per_cluster*(fat64_partition.cluster_tags)+fat64_partition.partition_location;
	uint32_t offset_into_sector = ((uint32_t)cluster)%cluster_tags_per_sector;
	uint8_t buf[512] = {0};
	fat64_cluster_tags_t * tags = (fat64_cluster_tags_t*)buf;

	//read in the sector
	ide_pio_lba_read(ide_device, sector_to_use, buf);

	//change the correct tag
	tags[offset_into_sector].prev = prev;
	tags[offset_into_sector].next = next;

	//write sector back
	ide_pio_lba_write(ide_device, sector_to_use, buf);
}

static void _fat64_user_init(context_t * context) {
	fat64_partition_t * partition = ARG(context,2);
	/*uint32_t i = 0, j;
	while(1) {
		c_printf("ARG(context,%x): %x\n",i,ARG(context,i));
		++i;
		for(j = 0; j < 0x10000000; ++j);
	}*/

#ifdef FAT64_DEBUG_MSG
	c_printf("fat64_user_init syscall hit\n");
#endif

	partition->root_dir = fat64_partition.root_dir;
	partition->cluster_tags = fat64_partition.cluster_tags;
	partition->size = fat64_partition.size;
	partition->cluster_size = fat64_partition.cluster_size;
	partition->partition_location = fat64_partition.partition_location;

	context->eax = E_SUCCESS;
}

//generic functions
status_t fat64_user_init(fat64_partition_t * partition);

status_t fat64_open(handle_t file, char * path) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;

	//open the root directory
	open_root(f);

	//go through the file system to the correct file
	//@TODO

	return E_SUCCESS;
}

status_t fat64_reopen(handle_t file) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;

	load_cluster(f);
}

status_t fat64_close(handle_t file) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;

	//check to see if data is dirty
	save_cluster(f); //write data to disk
	
	//clear out handle
	memset(f, 0, sizeof(fat64_file_t));

	return E_SUCCESS;
}

status_t fat64_is_directory(handle_t file, uint8_t * ret) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;

	//check for invalid handle
	if(f->cluster == 0)
		return FAT64_BAD_HANDLE;

	*ret = 0;
	if(f->entry.flags & FAT64_DIRECTORY_FLAG)
		*ret = 1;

	return E_SUCCESS;
}

//file functions
status_t fat64_rm(handle_t file) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;

	//check for invalid handle
	if(f->cluster == 0)
		return FAT64_BAD_HANDLE;

	//mark entry as invalid
	f->entry.flags = 0;
	save_dir_entry(f);

	//loop through the tags and mark them as free
	f->cluster = f->entry.file;
	while(f->cluster != FAT64_CLUSTER_TAG_EOF) {
		mark_cluster(f->cluster, FAT64_CLUSTER_TAG_FREE, FAT64_CLUSTER_TAG_FREE);
		f->cluster = f->current_tags.next;
		load_cluster_tags(f);
	}

	return E_SUCCESS;
}

status_t fat64_getc(handle_t file, uint8_t * data) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;

	//check for invalid handle
	if(f->cluster == 0)
		return FAT64_BAD_HANDLE;

	//check for EOF
	if(f->location == FAT64_EOF)
		return FAT64_EOF;

	//read in byte
	*data = f->buf[f->location];
	++(f->location);
	++(f->abs_location);

	//check for end of cluster and move to next one if valid
	if(f->location == FAT64_CLUSTER_SIZE) {
		//save current cluster
		save_cluster(f);

		if(f->current_tags.next != FAT64_CLUSTER_TAG_EOF) {
			//move to next cluster
			f->cluster = f->current_tags.next;
			open_cluster(f);

			//reset local location
			f->location = 0;
		} else
			f->location == FAT64_EOF;
	}

	return E_SUCCESS;
}

status_t fat64_putc(handle_t file, uint8_t data) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;
	uint64_t new_cluster = 0;
	uint64_t prev_cluster = 0;

	//check for invalid handle
	if(f->cluster == 0)
		return FAT64_BAD_HANDLE;

	//check for EOF
	if(f->location == FAT64_EOF) {
		//if it is a directory, return EOF
		if(f->entry.flags & FAT64_DIRECTORY_FLAG)
			return FAT64_EOF;

		//try to grab a new cluster
		prev_cluster = f->cluster;
		new_cluster = get_free_cluster(prev_cluster, FAT64_CLUSTER_TAG_EOF);

		//if it is an invalid new cluster, EOF
		if(new_cluster == FAT64_CLUSTER_TAG_EOF)
			return FAT64_EOF;

		//update the current tags and save them
		f->current_tags.next = new_cluster;
		save_cluster_tags(f);

		//save the current cluster information
		save_cluster(f);

		//move to next cluster in file
		f->cluster = f->current_tags.next;
		open_cluster(f);

		//reset local location
		f->location = 0;
	}

	//write data to buffer
	f->buf[f->location] = data;
	++(f->location);
	++(f->abs_location);
	f->dirty = 1;
	if((f->abs_location > f->entry.size)/* && (f->location != FAT64_CLUSTER_SIZE)*/) {
		++(f->entry.size);
		save_dir_entry(f);
	}

	//check for end of cluster and move to next one if valid
	if(f->location == FAT64_CLUSTER_SIZE) {
		//save current cluster
		save_cluster(f);

		if(f->current_tags.next != FAT64_CLUSTER_TAG_EOF) {
			//move to next cluster
			f->cluster = f->current_tags.next;
			open_cluster(f);

			//reset local location
			f->location = 0;
		} else
			f->location = FAT64_EOF;
	}
	return E_SUCCESS;
}

status_t fat64_read(handle_t file, uint64_t amount, uint8_t * buf) {
	uint32_t i = 0;
	status_t stat;
	for(i = 0; i < amount; ++i)
		if(stat = fat64_getc(file, buf+i))
			return stat;
	return E_SUCCESS;
}

status_t fat64_write(handle_t file, uint64_t amount, uint8_t * buf) {
	uint32_t i = 0;
	status_t stat;
	for(i = 0; i < amount; ++i)
		if(stat = fat64_putc(file, buf[i]))
			return stat;
	return E_SUCCESS;
}

status_t fat64_flush(handle_t file) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;

	save_cluster(f);
}

//directory functions
status_t fat64_dir_entry(handle_t dir, uint64_t index, handle_t file) {
	//change the handle to a pointer
	fat64_file_t * d = (fat64_file_t*)dir;
	fat64_file_t * f = (fat64_file_t*)file;
	fat64_dir_entry_t * entries = (fat64_dir_entry_t*)d->buf;

	//load the data from disk in case it is different
	load_cluster(d);

	//check to see if the entry is valid or not
	if((index > 8) || !(entries[index].flags & FAT64_VALID_FLAG)) {
		return FAT64_EOF;
	}

	//set the file handle memory
	memset(f, 0, sizeof(fat64_file_t));

	//copy over directory entry info
	memcpy(&(f->entry), &(entries[index]), sizeof(fat64_dir_entry_t));

	//set the cluster
	f->cluster = f->entry.file;
	open_cluster(f);

	return E_SUCCESS;
}

status_t fat64_touch(handle_t dir, char * name) {
	//change the handle to a pointer
	fat64_file_t * d = (fat64_file_t*)dir;
	fat64_file_t f;
	uint64_t entry_num = get_free_dir_entry(d);

	load_cluster(d);

	if(d == FAT64_EOF)
		return FAT64_EOF;

	//clear it out initially
	memset(&f, 0, sizeof(fat64_file_t));

	//setup file handle infor for internal use
	f.cluster = get_free_cluster(FAT64_CLUSTER_TAG_EOF, FAT64_CLUSTER_TAG_EOF);

	//setup entry information
	strcpy(f.entry.name, name);
	f.entry.file = f.cluster;
	f.entry.parent = d->entry.file;
	f.entry.size = 0;
	f.entry.flags = FAT64_VALID_FLAG;
	f.entry.dir_entry_num = entry_num;

	//save directory information
	save_dir_entry(&f);

	return E_SUCCESS;
}

status_t fat64_mkdir(handle_t dir, char * name) {
	//change the handle to a pointer
	fat64_file_t * d = (fat64_file_t*)dir;
	fat64_file_t f;
	uint64_t entry_num = get_free_dir_entry(d);

	if(d == FAT64_EOF)
		return FAT64_EOF;

	//clear it out initially
	memset(&f, 0, sizeof(fat64_file_t));

	//setup file handle infor for internal use
	f.cluster = get_free_cluster(FAT64_CLUSTER_TAG_EOF, FAT64_CLUSTER_TAG_EOF);
	f.dirty = 1; //mark it as dirty to save the cluster info, ie make it zeros

	//setup entry information
	strcpy(f.entry.name, name);
	f.entry.file = f.cluster;
	f.entry.parent = d->entry.file;
	f.entry.size = FAT64_CLUSTER_SIZE;
	f.entry.flags = FAT64_VALID_FLAG | FAT64_DIRECTORY_FLAG;
	f.entry.dir_entry_num = entry_num;

	//save directory information
	save_dir_entry(&f);
	save_cluster(&f);

	return E_SUCCESS;
}

//helper functions
void open_root(fat64_file_t * f) {
	//root entry info
	memset(f, 0, sizeof(fat64_file_t));
	f->entry.name[0] = '/';
	f->entry.file = fat64_partition.root_dir;
	f->entry.parent = f->entry.file;
	f->entry.size = FAT64_CLUSTER_SIZE;
	f->entry.time_created = 0;
	f->entry.time_accessed = 0;
	f->entry.time_modified = 0;
	f->entry.flags = FAT64_VALID_FLAG | FAT64_DIRECTORY_FLAG;
	f->entry.dir_entry_num = 0;

	//file handle info
	f->cluster = fat64_partition.root_dir;
	f->abs_location = 0;
	f->location = 0;
	f->dirty = 0;

	//load in cluster data to file handle
	open_cluster(f);
}

void load_cluster(fat64_file_t * f) {
	//number if sectors per cluster
	uint32_t number_sectors = FAT64_CLUSTER_SIZE / SECTOR_SIZE;
	uint32_t sector_to_read = 0;
	uint32_t i = 0;

#ifdef FAT64_DEBUG_MSG
	//c_printf("Cluster Read: 0x%x:\n",f->cluster);
#endif

	//loop through the sectors in the cluster and read them in
	for(i = 0; i < number_sectors; ++i) {
		sector_to_read = number_sectors*f->cluster+i+
						 fat64_partition.partition_location;
		ide_read(sector_to_read, f->buf+SECTOR_SIZE*i);
	}
}

void save_cluster(fat64_file_t * f) {
	//number if sectors per cluster
	uint32_t number_sectors = FAT64_CLUSTER_SIZE / SECTOR_SIZE;
	uint32_t sector_to_write = 0;
	uint32_t i = 0;

	if(!(f->dirty))
		return;

#ifdef FAT64_DEBUG_MSG
	//c_printf("Cluster Write: 0x%x:\n",f->cluster);
#endif

	//loop through the sectors in the cluster and read them in
	for(i = 0; i < number_sectors; ++i) {
		sector_to_write = number_sectors*f->cluster+i+
						 fat64_partition.partition_location;
		ide_write(sector_to_write, f->buf+SECTOR_SIZE*i);
	}

	f->dirty = 0;
}

void load_dir_entry(fat64_file_t * f) {
	fat64_file_t dir;
	fat64_dir_entry_t * entries;

	dir.cluster = f->entry.parent;
	open_cluster(&dir);
	entries = (fat64_dir_entry_t*)dir.buf;

	memcpy(&(f->entry), &(entries[f->entry.dir_entry_num]), sizeof(fat64_dir_entry_t));
}

void save_dir_entry(fat64_file_t * f) {
	fat64_file_t dir;
	fat64_dir_entry_t * entries;

	dir.cluster = f->entry.parent;
	open_cluster(&dir);
	entries = (fat64_dir_entry_t*)dir.buf;

	memcpy(&(entries[f->entry.dir_entry_num]), &(f->entry), sizeof(fat64_dir_entry_t));
	dir.dirty = 1;

	save_cluster(&dir);
}

void open_cluster(fat64_file_t * f) {
	//load in the cluster data
	load_cluster(f);

	//get the cluster tags
	load_cluster_tags(f);
}

void load_cluster_tags(fat64_file_t * f) {
	uint32_t tags_per_sector = SECTOR_SIZE/sizeof(fat64_cluster_tags_t);
	uint32_t sectors_per_cluster = FAT64_CLUSTER_SIZE / SECTOR_SIZE;
	uint32_t sector_to_read = (uint32_t)f->cluster / tags_per_sector + //gets the offset into the tag tabls in sectors
							  fat64_partition.cluster_tags * sectors_per_cluster + //gets the offset of tag table
							  fat64_partition.partition_location; //offset into disk partition is
	uint32_t which_tag = (uint32_t)f->cluster % tags_per_sector; //gets the index into the sector
	uint8_t buf[SECTOR_SIZE] = {0}; //get a buffer
	fat64_cluster_tags_t * tags = (fat64_cluster_tags_t *)buf; //make the sector array be the same as buffer

	//read in the sectors
	ide_read(sector_to_read, buf);

	//set the tags in the file handle
	f->current_tags.prev = tags[which_tag].prev;
	f->current_tags.next = tags[which_tag].next;
}

void save_cluster_tags(fat64_file_t * f) {
	uint32_t tags_per_sector = SECTOR_SIZE/sizeof(fat64_cluster_tags_t);
	uint32_t sectors_per_cluster = FAT64_CLUSTER_SIZE / SECTOR_SIZE;
	uint32_t sector_to_read = (uint32_t)f->cluster / tags_per_sector + //gets the offset into the tag tabls in sectors
							  fat64_partition.cluster_tags * sectors_per_cluster + //gets the offset of tag table
							  fat64_partition.partition_location; //offset into disk partition is
	uint32_t which_tag = (uint32_t)f->cluster % tags_per_sector; //gets the index into the sector
	uint8_t buf[SECTOR_SIZE] = {0}; //get a buffer
	fat64_cluster_tags_t * tags = (fat64_cluster_tags_t *)buf; //make the sector array be the same as buffer

	//read in the sectors
	ide_read(sector_to_read, buf);

	//set the tags in the file handle
	tags[which_tag].prev = f->current_tags.prev;
	tags[which_tag].next = f->current_tags.next;

	//write changes to disk
	ide_write(sector_to_read,buf);
}

uint64_t get_free_cluster(uint64_t prev, uint64_t next) {
	uint64_t free_cluster = 0;
	uint32_t sector_to_read = 0;
	uint32_t which_tag = 0;
	uint32_t tags_per_sector = SECTOR_SIZE/sizeof(fat64_cluster_tags_t);
	uint32_t sectors_per_cluster = FAT64_CLUSTER_SIZE / SECTOR_SIZE;
	uint8_t buf[SECTOR_SIZE] = {0}; //get a buffer
	fat64_cluster_tags_t * tags = (fat64_cluster_tags_t *)buf; //make the sector array be the same as buffer

	for(free_cluster = 0; free_cluster < fat64_partition.size; ++free_cluster) {
		which_tag = (uint32_t)free_cluster % tags_per_sector;
		if(!which_tag) {
			sector_to_read = (uint32_t)free_cluster / tags_per_sector + //gets the offset into the tag tabls in sectors
							  fat64_partition.cluster_tags * sectors_per_cluster + //gets the offset of tag table
							  fat64_partition.partition_location; //offset into disk partition is
			ide_read(sector_to_read, buf); //read the sector in
		}
#ifdef FAT64_DEBUG_MSG
		c_printf("Trying Cluster: %x\n", free_cluster);
#endif
		
		//check for free cluster
		if(tags[which_tag].prev == FAT64_CLUSTER_TAG_FREE &&
		   tags[which_tag].next == FAT64_CLUSTER_TAG_FREE) {
			break;
		}
	}

	//if the cluster is not in table, return EOF
	if(free_cluster >= fat64_partition.size)
		return FAT64_CLUSTER_TAG_EOF;

	//mark the cluster
	mark_cluster(free_cluster, prev, next);

	return free_cluster;
}

uint64_t get_free_dir_entry(fat64_file_t * d) {
	fat64_dir_entry_t * entries = (fat64_dir_entry_t*)d->buf;
	uint32_t i;

	for(i = 0; i < 8; ++i)
		if(!(entries[i].flags & FAT64_VALID_FLAG))
			break;

	if(i == 8)
		return FAT64_EOF;

	return i;
}

void mark_cluster(uint64_t cluster, uint64_t prev, uint64_t next) {
	uint32_t cluster_tags_per_sector = SECTOR_SIZE/sizeof(fat64_cluster_tags_t);
	uint32_t sectors_per_cluster = FAT64_CLUSTER_SIZE/SECTOR_SIZE;
	uint32_t sector_to_use = ((uint32_t)cluster)/cluster_tags_per_sector+sectors_per_cluster*(fat64_partition.cluster_tags)+fat64_partition.partition_location;
	uint32_t offset_into_sector = ((uint32_t)cluster)%cluster_tags_per_sector;
	uint8_t buf[512] = {0};
	fat64_cluster_tags_t * tags = (fat64_cluster_tags_t*)buf;

	//read in the sector
	ide_read(sector_to_use, buf);

	//change the correct tag
	tags[offset_into_sector].prev = prev;
	tags[offset_into_sector].next = next;

	//write sector back
	ide_write(sector_to_use, buf);
}
