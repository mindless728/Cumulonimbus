#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "riff.h"

//int readChunk(int fd, struct RiffChunk* ch);

int main(int argc, char** argv){


	if(argc != 2){
		printf("Usage: %s [file]\n", argv[0]);
		return(-1);
	}

	char* filename = argv[1];
	int fd = open(filename, O_CLOEXEC);

	if(fd == -1){
		perror("open");
		return(0);
	}

	struct RiffType* type = NULL;

	while(read_riff_type(fd, &type) > 0){
		if(type != NULL){
			printf("Read header id=");
			print_riff_text(type->hdr.id);
			printf("\n");
		}
	}

	/*
	int i=0;
	struct RiffChunk ch;
	while(readChunk(fd, &ch) > 0 && i<10){
		printf(("Chunk%i:\tsize=%i FRMT="), i, ch.size);

		int j = 0;
		for(; j<3; j++){
			printf("%c", ch.format[j]);
		}

		printf("ID=");

		j = 0;
		for(; j<3; j++){
			printf("%c", ch.id[j]);
		}


		printf("\n");

		i++;

		//lseek(fd, ch.size, SEEK_CUR);
	}

	printf("Done\n");
	*/
}

/*int readChunk(int fd, struct RiffChunk* ch){
	return read(fd, (void*) ch, sizeof(struct RiffChunk));
*/
	/*printf("ID:\t ");
	int i=0;
	for(; i<4; i++){
		printf("%c", chunk.id);
	}
	printf("\n");*/
//}

