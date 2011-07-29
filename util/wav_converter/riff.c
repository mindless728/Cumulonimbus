#include "riff.h"

#include <unistd.h>
#include <string.h>

int read_riff_header(int fd, struct RiffHeader* hdr){
	return read(fd, (void*) hdr, sizeof(struct RiffHeader));
}


int read_riff_type(int fd, struct RiffType** hdr){
	int status=0;
	struct RiffHeader temp;

	status = read_riff_header(fd, &temp);

	if(status > 0){
		uint32_t bytes=0;
		if(memcmp(temp.id, "RIFF", 4) == 0){
			bytes = sizeof(struct RiffType);
			*hdr = (struct RiffHeader*) malloc(bytes);
			status = read_riff_struct(fd, &temp, *hdr, bytes);
		}
		else if(memcmp(temp.id, "fmt ", 4) == 0){
			bytes = sizeof(struct FMTChunk);
			*hdr = (struct RiffHeader*) malloc(bytes);

			status = read_riff_struct(fd, &temp, *hdr, 16+sizeof(struct RiffHeader));
			if(status < 1){ return status; }

			struct FMTChunk* fmt = *hdr;
 			//Check for extra fields
			if(fmt->audioFormat != 0){
				//Read extra fields
				
				status = read(fd, (void*) &fmt->extraBytes, sizeof(uint16_t));
				
				if(status < 1){ return status; }

				if(fmt->extraBytes > 0){
					fmt->extra = (uint8_t*) malloc(fmt->extraBytes);
					status = read(fd, (void*) fmt->extra, fmt->extraBytes);
				}
			}
		}
		else{
			printf("__PRETTY_FUNCION__ = WARNING: Unknown header type id=");
			print_riff_text(temp.id);
			printf("\n");
		}

	}
	else{
		printf("__PRETTY_FUNCTION__ - ERROR: Failed to read\n");
		*hdr = NULL;
	}
	return status;
}


void copy_riff_header(struct RiffHeader* src, struct RiffHeader* dst){
	memcpy(dst, src, sizeof(struct RiffHeader));
}


void print_riff_text(uint8_t* text){
	printf("%c%c%c%c", text[0], text[1], text[2], text[3]);
}


int read_riff_struct(int fd, struct RiffHeader* hdr, void* data, uint32_t bytes){
	if(bytes < sizeof(struct RiffHeader)){
		return -1;
	}

	copy_riff_header(hdr, (struct RiffHeader*) data);

	return read(fd, data, bytes - sizeof(struct RiffHeader));
}
