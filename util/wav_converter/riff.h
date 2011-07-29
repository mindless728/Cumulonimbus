#ifndef _RIFF_H
#define _RIFF_H

#include <stdint.h>
#include <stdlib.h>

struct RiffHeader{
	uint8_t		id[4];
	uint32_t	size;
};


struct RiffType {
	struct RiffHeader	hdr;
	uint8_t		format[4];
};


struct FMTChunk {
	union {
		struct{
			uint8_t		subChunkID[4];
			uint32_t	subChunkSize;
		};

		struct RiffHeader	hdr;
	};

	uint16_t	audioFormat;
	uint16_t	numChannels;
	uint32_t	sampleRate;
	uint32_t	avgBytesPerSecond;
	uint16_t	blockAlign;
	uint16_t	bitsPerSample;

	//Optional fields if(audioFormat != 0)
	uint16_t	extraBytes;
	uint8_t*	extra;
};


struct DataChunk {
	union {
		struct {
			uint8_t		subChunkID[4];
			uint32_t	subChunkSize;
		};

		struct RiffHeader	hdr;
	};

	uint8_t*	data;
};


int read_riff_header(int fd, struct RiffHeader* hdr);

int read_riff_type(int fd, struct RiffType** ch);

void copy_riff_header(struct RiffHeader* src, struct RiffHeader* dst);

void print_riff_text(uint8_t* field);

int read_riff_struct(int fd, struct RiffHeader* hdr, void* data, uint32_t bytes);
#endif	//_RIFF_H
