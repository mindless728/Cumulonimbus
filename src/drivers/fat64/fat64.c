#include "fat64.h"

#define FAT64_FS_SIZE 0x40000000
//#define MKFS_DEBUG

ide_device_t * ide_device;
fat64_partition_t fat64_partition;

//kernel helper functions
void _mark_cluster(uint64_t cluster, uint64_t prev, uint64_t next);

//user space helper functions
uint64_t get_free_cluster(void);
void mark_cluster(uint64_t cluster, uint64_t prev, uint64_t next);
void get_cluster_tag(uint64_t cluster, fat64_cluster_tags_t * tag);
void move_to_next_cluster(fat64_file_t * f);
void move_to_prev_cluster(fat64_file_t * f);
void load_cluster(int64_t cluster, uint8_t * buf);
void save_cluster(int64_t cluster, uint8_t * buf);
void update_dir_entry(fat64_file_t * f);
uint64_t get_free_dir_entry(fat64_file_t * d);

//note does not fill in f->entry information!!!
void open_by_cluster(fat64_file_t * f, uint64_t cluster);

//kernel functions
status_t _fat64_init(void) {
	ide_device = &(ide_devices[1]);
	mbr_t mbr;

	//check for existing FS on disk
	ide_pio_lba_read(ide_device,0,(uint8_t*)&mbr);
	if(((mbr.partition[0].status != 0x00) && (mbr.partition[0].status != 0x80)) || (mbr.MBR_signature != MBR_SIG)) {
		//if it doesn't create it
		_fat64_mkfs(FAT64_FS_SIZE, &mbr);
	} else
		c_printf("FAT64 Filesystem Found!\n");

	if(mbr.partition[0].partition_type != FAT64_PARTITION_TYPE) {
		c_printf("***ERROR*** FAT64 Filesystem not found - wroing type in MBR");
		return E_ERROR;
	}

	//read in FS specific partition info
	ide_pio_lba_read(ide_device,mbr.partition[0].first_LBA,(uint8_t*)&fat64_partition);

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
	fat64_partition.size = mbr->partition[0].num_sectors*SECTOR_SIZE/FAT64_CLUSTER_SIZE;
	fat64_partition.cluster_size = FAT64_CLUSTER_SIZE;
#ifdef MKFS_DEBUG
	c_printf("Cluster Tag Size (Clusters): 0x%x\n",cluster_tag_size);
	c_printf("Root Directory Cluster: 0x%x\n",fat64_partition.root_dir);
	c_printf("Cluster Tag Location Cluster: 0x%x\n",fat64_partition.cluster_tags);
	c_printf("Partition Cluster Size: 0x%x\n",fat64_partition.size);
#endif
	
	ide_pio_lba_write(ide_device,start_lba,(uint8_t*)&fat64_partition);

	//mark the superblock cluster as used
	_mark_cluster(0,FAT64_CLUSTER_TAG_EOF,FAT64_CLUSTER_TAG_EOF);

	//create cluster tag table
	for(i = 0; i < cluster_tag_size; ++i) {
		//mark cluster as allocated with prev and next set
		prev = i - 1;
		if(i == 0)
			prev = FAT64_CLUSTER_TAG_EOF;
		next = i + 1;
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
	uint32_t sector_to_use = ((uint32_t)cluster)/cluster_tags_per_sector+sectors_per_cluster*(fat64_partition.cluster_tags+1);
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

//generic functions
status_t fat64_user_init(void) {
	//call the system call to init the user space fat64
	//  giving the pointer to the partition info to fill in
}

status_t fat64_open(handle_t file, char * path) {
}

status_t fat64_close(handle_t file) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;

	//check for bad handle
	if(f->cluster == FAT64_CLUSTER_TAG_EOF)
		return FAT64_BAD_HANDLE;

	//if the dirty bit is set, flush data to disk
	if(f->dirty)
		fat64_flush(file);

	//dirty the data to make it unusable
	memset(f, 0, sizeof(fat64_file_t));
	f->cluster = FAT64_CLUSTER_TAG_EOF;
}

status_t fat64_is_directory(handle_t file, uint8_t * ret) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;

	//test to see if it is a directory
	*ret = f->entry.flags & FAT64_DIRECTORY_FLAG;

	return E_SUCCESS;
}

//file functions
status_t fat64_rm(handle_t file) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;
	uint64_t cluster = 0;
	fat64_cluster_tags_t tags;

	//change the flags for the entry
	f->entry.flags &= ~FAT64_VALID_FLAG;

	//update the entry
	update_dir_entry(f);

	//free clusters for file
	cluster = f->entry.file;
	while(cluster != FAT64_CLUSTER_TAG_EOF) {
		get_cluster_tag(cluster, &tags);
		mark_cluster(cluster, FAT64_CLUSTER_TAG_FREE, FAT64_CLUSTER_TAG_FREE);
		cluster = tags.next;
	}

	return E_SUCCESS;
}

status_t fat64_mv(handle_t file, handle_t dir) {
}

status_t fat64_getc(handle_t file, uint8_t * data) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;

	//check for bad handle
	if((f->cluster == FAT64_CLUSTER_TAG_EOF))
		return FAT64_BAD_HANDLE;

	//check to see if EOF
	if((f->location == FAT64_CLUSTER_SIZE) || (f->abs_location == f->entry.size))
		return FAT64_EOF;

	//get the data
	*data = f->buf[f->location];

	//increment the location
	++(f->location);
	++(f->abs_location);

	//check to see if location is out of bounds and
	//  has a next cluster
	if((f->location == FAT64_CLUSTER_SIZE) &&
	   (f->current_tags.next != FAT64_CLUSTER_TAG_EOF)) {
		//switch to next cluster in file
		move_to_next_cluster(f);
		f->location = 0;
	}

	return E_SUCCESS;
}

status_t fat64_putc(handle_t file, uint8_t data) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;
	uint64_t new_cluster;

	//check for bad handle
	if(f->cluster == FAT64_CLUSTER_TAG_EOF)
		return FAT64_BAD_HANDLE;

	//check to see if EOF
	if(f->location == FAT64_CLUSTER_SIZE) {
		//try to obtain a new cluster for the file
		new_cluster = get_free_cluster();

		//if successful
		if(new_cluster != FAT64_CLUSTER_TAG_EOF) {
			//mark the clusters accordingly
			mark_cluster(f->cluster, f->current_tags.prev, new_cluster);
			mark_cluster(new_cluster, f->cluster, FAT64_CLUSTER_TAG_EOF);
			f->current_tags.next = new_cluster;

			//move to the next cluster in the file
			move_to_next_cluster(f);
			memset(f->buf, 0, FAT64_CLUSTER_SIZE);
			f->dirty = 1;
		} else {
			//if unsuccessful, return EOF
			return FAT64_EOF;
		}
	}

	//set the data
	f->buf[f->location] = data;
	f->dirty = 1;

	//increment location
	++(f->location);
	++(f->abs_location);

	//check to see if location is out of bounds 
	if((f->location == FAT64_CLUSTER_SIZE) &&
	   (f->current_tags.next != FAT64_CLUSTER_TAG_EOF)) {
		//switch to next cluster in file
		move_to_next_cluster(f);
		f->location = 0;
	}

	//check to see if file size increased
	if(f->abs_location > f->entry.size) {
		//if so increase size by one
		++(f->entry.size);
		//write out the changed entry
		update_dir_entry(f);
	}

	return E_SUCCESS;
}

status_t fat64_read(handle_t file, uint64_t amount, uint8_t * buf) {
	uint64_t i = 0;
	status_t status;

	//loop through and read each byte at a time
	//  this is ok since it is buffered
	for(i = 0; i < amount; ++i)
		//check for an EOF
		if((status = fat64_getc(file,&buf[i])))
			return status; //return if EOF

	return E_SUCCESS;
}

status_t fat64_write(handle_t file, uint64_t amount, uint8_t * buf) {
	uint64_t i = 0;
	status_t status;

	//loop through and write each byte at a time
	//  this is ok since it is buffered
	for(i = 0; i < amount; ++i)
		if((status = fat64_putc(file,buf[i])))
			return status;

	return E_SUCCESS;
}

status_t fat64_seek(handle_t file, int64_t offset, uint8_t type) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;

	//if seeking from the current position
	if(type == FAT64_SEEK_CUR) {
		
	//if seeking to a position into a file
	} else if(type == FAT64_SEEK_BEG) {
		open_by_cluster(f, f->entry.file);
		fat64_seek(file, offset, FAT64_SEEK_CUR);
	//if seeking from the end of a file
	} else if(type == FAT64_SEEK_END) {
	}
}

status_t fat64_tell(handle_t file, uint64_t * offset) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;

	//check for bad handle
	if(f->cluster == FAT64_CLUSTER_TAG_EOF)
		return FAT64_BAD_HANDLE;

	//get the offset into the file
	*offset = f->abs_location;
}

status_t fat64_flush(handle_t file) {
	//change the handle into the file
	fat64_file_t * f = (fat64_file_t*)file;

	//check for bad handle
	if(f->cluster == FAT64_CLUSTER_TAG_EOF)
		return FAT64_BAD_HANDLE;

	uint32_t i = 0;
	for(i = 0; i < 8; ++i)
		ide_write(8*(f->cluster)+fat64_partition.partition_location+i,((uint8_t*)f->buf)+512*i);

	//@TODO update the modify time
	f->entry.time_modified = f->entry.time_modified;

	//write the entry to disk
	update_dir_entry(f);
}


//directory functions
status_t fat64_dir_entry(handle_t dir, uint64_t index, handle_t file) {
	//change the handle to a pointer
	fat64_file_t * f = (fat64_file_t*)file;

	//check for bad handle
	if(f->cluster == FAT64_CLUSTER_TAG_EOF)
		return FAT64_BAD_HANDLE;

	//seek into the directory to the placement
	fat64_seek(dir, sizeof(fat64_dir_entry_t)*index, FAT64_SEEK_BEG);

	//read in the dir_entry to the file handle
	fat64_read(dir, sizeof(fat64_dir_entry_t), (uint8_t*)&(f->entry));

	//set the current cluster
	f->cluster = f->entry.file;

	//set the current cluster tags
	get_cluster_tag(f->cluster, &(f->current_tags));

	//load in the initial cluster
	load_cluster(f->entry.file, f->buf);

	//set other initial values
	f->abs_location = 0;
	f->location = 0;
	f->dirty = 0;
}

status_t fat64_mkdir(handle_t dir, char * name) {
	//change the handle to a pointer
	fat64_file_t * d = (fat64_file_t*)dir;
	fat64_file_t file;
	uint64_t cluster = get_free_cluster(); //get the first free cluster

	//get the first free directory entry
	uint64_t free_entry = get_free_dir_entry(d);

	//test to see if either are bad allocations
	if(cluster == FAT64_CLUSTER_TAG_EOF || free_entry == FAT64_CLUSTER_TAG_EOF)
		return FAT64_CLUSTER_TAG_EOF;

	//null out the file handle to the new file
	memset(&file,0,sizeof(fat64_file_t));

	//set up the entry info to be written to disk
	//@TODO copy name over
	file.entry.file = cluster;
	file.entry.parent = d->entry.file;
	file.entry.size = 0;
	//@TODO use real time here
	file.entry.time_created = file.entry.time_accessed = file.entry.time_modified = 0;
	file.entry.flags = FAT64_VALID_FLAG | FAT64_DIRECTORY_FLAG;
	file.entry.dir_entry_num = free_entry;

	//mark cluster as used
	mark_cluster(cluster, FAT64_CLUSTER_TAG_EOF, FAT64_CLUSTER_TAG_EOF);

	//save entry info to disk
	update_dir_entry(&file);
}

status_t fat64_touch(handle_t dir, char * name) {
	//change the handle to a pointer
	fat64_file_t * d = (fat64_file_t*)dir;
	fat64_file_t file;
	uint64_t cluster = get_free_cluster(); //get the first free cluster

	//get the first free directory entry
	uint64_t free_entry = get_free_dir_entry(d);

	//test to see if either are bad allocations
	if(cluster == FAT64_CLUSTER_TAG_EOF || free_entry == FAT64_CLUSTER_TAG_EOF)
		return FAT64_CLUSTER_TAG_EOF;

	//null out the file handle to the new file
	memset(&file,0,sizeof(fat64_file_t));

	//set up the entry info to be written to disk
	//@TODO copy name over
	file.entry.file = cluster;
	file.entry.parent = d->entry.file;
	file.entry.size = 0;
	//@TODO use real time here
	file.entry.time_created = file.entry.time_accessed = file.entry.time_modified = 0;
	file.entry.flags = FAT64_VALID_FLAG;
	file.entry.dir_entry_num = free_entry;

	//mark cluster as used
	mark_cluster(cluster, FAT64_CLUSTER_TAG_EOF, FAT64_CLUSTER_TAG_EOF);

	//save entry info to disk
	update_dir_entry(&file);
}

status_t fat64_rmdir(handle_t dir) {
}

//user land helper functions
uint64_t get_free_cluster(void) {
	uint32_t cluster_tags_per_sector = SECTOR_SIZE/sizeof(fat64_cluster_tags_t);
	uint64_t sectors_to_read = ((uint32_t)fat64_partition.size) / cluster_tags_per_sector;
	uint32_t i, j;
	uint8_t buf[512] = {0};
	fat64_cluster_tags_t * tags = (fat64_cluster_tags_t*)buf;

	for(i = 0; i < sectors_to_read; ++i) {
		ide_read(i+8*fat64_partition.cluster_tags+fat64_partition.partition_location, buf);
		for(j = 0; j < cluster_tags_per_sector; ++j) {
			if((tags[j].prev == FAT64_CLUSTER_TAG_FREE) &&
			   (tags[j].next == FAT64_CLUSTER_TAG_FREE))
				return i*cluster_tags_per_sector+j;
		}
	}

	return FAT64_CLUSTER_TAG_EOF;
}

void mark_cluster(uint64_t cluster, uint64_t prev, uint64_t next) {
	uint32_t cluster_tags_per_sector = SECTOR_SIZE/sizeof(fat64_cluster_tags_t);
	uint32_t sectors_per_cluster = FAT64_CLUSTER_SIZE/SECTOR_SIZE;
	uint32_t sector_to_use = ((uint32_t)cluster)/cluster_tags_per_sector+sectors_per_cluster*(fat64_partition.cluster_tags+1);
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

void get_cluster_tag(uint64_t cluster, fat64_cluster_tags_t * tag) {
	uint32_t cluster_tags_per_sector = SECTOR_SIZE/sizeof(fat64_cluster_tags_t);
	uint32_t sector_to_use = ((uint32_t)cluster)/cluster_tags_per_sector+
							 8*fat64_partition.cluster_tags+
							 fat64_partition.partition_location;
	uint32_t tag_number = ((uint32_t)cluster)%cluster_tags_per_sector;
	uint8_t buf[512] = {0};
	fat64_cluster_tags_t * tags = (fat64_cluster_tags_t*)buf;

	//read in the sector that has the tags
	ide_read(sector_to_use, buf);

	//get the tag information
	tag->prev = tags[tag_number].prev;
	tag->next = tags[tag_number].next;
}

void move_to_next_cluster(fat64_file_t * f) {
	//if the next cluster is invalid, don't do anything
	if(f->current_tags.next == FAT64_CLUSTER_TAG_EOF)
		return;

	//if the data is dirty, write it to disk
	if(f->dirty)
		fat64_flush((handle_t)f);

	//change over to the next cluster
	f->cluster = f->current_tags.next;

	//load in the cluster data
	load_cluster(f->cluster, f->buf);

	//get the new current cluster tags
	get_cluster_tag(f->cluster, &(f->current_tags));

	//make the data not dirty
	f->dirty = 0;
}

void move_to_prev_cluster(fat64_file_t * f) {
	//if the prev cluster is invalid, don't do anything
	if(f->current_tags.prev == FAT64_CLUSTER_TAG_EOF)
		return;

	//if the data is dirty, write it to disk
	if(f->dirty)
		fat64_flush((handle_t)f);

	//change over to the next cluster
	f->cluster = f->current_tags.prev;

	//load in the cluster data
	load_cluster(f->cluster, f->buf);

	//get the new current cluster tags
	get_cluster_tag(f->cluster, &(f->current_tags));

	//make the data not dirty
	f->dirty = 0;
}

void load_cluster(int64_t cluster, uint8_t * buf) {
	uint32_t sectors_per_cluster = FAT64_CLUSTER_SIZE/SECTOR_SIZE;
	uint32_t i;

	//loop through the sectors in the cluster
	for(i = 0; i < sectors_per_cluster; ++i) {
		//for each sector load it from disk
		ide_read(8*(cluster+fat64_partition.cluster_tags)+fat64_partition.partition_location+i, &(buf[SECTOR_SIZE*i]));
	}
}

void update_dir_entry(fat64_file_t * f) {
	fat64_file_t d;
	handle_t dir = (handle_t)(&d);

	//open directory by cluster number
	open_by_cluster(&d, f->entry.parent);

	//seek to the correct position of the file
	fat64_seek(dir, sizeof(fat64_dir_entry_t)*f->entry.dir_entry_num, FAT64_SEEK_BEG);

	//write out the entry information
	fat64_write(dir, sizeof(fat64_dir_entry), (uint8_t *)&(f->entry));
}

void open_by_cluster(fat64_file_t * f, uint64_t cluster) {
	//get the cluster tags
	get_cluster_tag(cluster, &(f->current_tags));

	//null out entry num as we don't know it here
	f->entry_num = 0;

	//set current cluster
	f->cluster = cluster;

	//set current file position
	f->abs_location = 0;

	//set location into current cluster
	f->location = 0;

	//set dirty to false
	f->dirty = 0;

	//load in cluster
	load_cluster(cluster, f->buf);
}

uint64_t get_free_dir_entry(fat64_file_t * d) {
	fat64_dir_entry_t dir_entry;
	uint64_t free_entry = 0;
	uint64_t new_cluster = 0;

	//seek to the beginning of the file
	fat64_seek((handle_t)d, 0, FAT64_SEEK_BEG);

	//loop through until you find one
	while(1) {
		//read in a directory entry
		fat64_read((handle_t)d, sizeof(fat64_dir_entry_t), (uint8_t*)&dir_entry);

		//test to see if it is free
		if(!(dir_entry.flags & FAT64_VALID_FLAG))
			break;

		//it isn't free, move on to the next
		++free_entry;

		//test to see if you are at the end of the directory file
		if(d->abs_location == d->entry.size) {
			//try to obtain new cluster
			new_cluster = get_free_cluster();

			//test to see if it is valid
			if(new_cluster != FAT64_CLUSTER_TAG_EOF) {
				//mark the clusters accordingly
				mark_cluster(d->cluster, d->current_tags.prev, new_cluster);
				mark_cluster(new_cluster, d->cluster, FAT64_CLUSTER_TAG_EOF);
				d->current_tags.next = new_cluster;

				//move to the next cluster in the file
				move_to_next_cluster(d);

				//set the cluster to null and dirty
				memset(d->buf, 0, FAT64_CLUSTER_SIZE);
				d->dirty = 1;
			} else {
				//no free dir_entry
				return FAT64_CLUSTER_TAG_EOF;
			}
		}
	}

	return free_entry;
}
