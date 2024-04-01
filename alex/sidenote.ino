/*serialize.h means this
#ifndef __SERIALIZE__ //ensures that compilers and system libraries can define symbols that won't collide with yours.//
#define SERIALIZE

#include <stdlib.h>

#define PACKET_SIZE		140
#define MAX_DATA_SIZE				128

typedef enum
{
	PACKET_OK = 0,
	PACKET_BAD = 1,
	PACKET_CHECKSUM_BAD = 2,
	PACKET_INCOMPLETE = 3,
	PACKET_COMPLETE = 4
} TResult;//TResult type////can do TResult result = PACKET_OK//

int serialize(char *buffer, void *dataStructure, size_t size);
TResult deserialize(const char *buffer, int len, void *output);//returns 0 - 4 

#endif

#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "serialize.h"

#define MAGIC_NUMBER				0xFCFDFEFF

typedef struct comms
{
	uint32_t magic;
	uint32_t dataSize;
	char buffer[MAX_DATA_SIZE];//128 defined serailized.h//
	unsigned char checksum;
	char dummy[3];
} TComms;

static char _privateBuffer[PACKET_SIZE];//defined as 140 in serialized.h//

static TResult assemble(char *outputBuffer, const char *inputBuffer, int len)//assemble() returns whether packet received is complete or not//
{
	// For copying to output buffer
	static int counter=0;

	// If there's leftover bytes from the next transmission
	static int leftoverFlag=0;
	static int leftoverCount=0;
	static char leftoverBuffer[PACKET_SIZE];

	int bytesLeft;
	int i;	

	// Copy in leftovers
	if(leftoverFlag)
	{
		int copyCount;
		if(leftoverCount <= PACKET_SIZE)
		{
			leftoverFlag=0;
			copyCount = leftoverCount;
		}
		else
			copyCount = PACKET_SIZE;

		leftoverCount -= copyCount;

		for(i=0; i<copyCount; i++)
		{
			outputBuffer[counter++] = leftoverBuffer[i];
		}
	}

	if(counter + len >= PACKET_SIZE)
	{
		bytesLeft = (PACKET_SIZE - counter);
		leftoverFlag=1;
		int bytesToCopy = len - bytesLeft;

		// Copy to leftover buffer
		for(i=0; i<bytesToCopy; i++)
		{
			leftoverBuffer[leftoverCount+i] = inputBuffer[bytesLeft + i];
		}
		leftoverCount += bytesToCopy;
	}
	else
		bytesLeft = len;

	for(i=0; i<bytesLeft; i++)
		outputBuffer[counter++] = inputBuffer[i];

	if(counter == PACKET_SIZE)
	{
		counter = 0;
		return PACKET_COMPLETE;
	}
	else
		return PACKET_INCOMPLETE;
}


TResult deserialize(const char *buffer, int len, void *output)
{
	TResult result = assemble(_privateBuffer, buffer, len);//checks whether packet is complete//

	if(result == PACKET_COMPLETE)
	{
		// Extract out the comms packet
		TComms *comms = (TComms *) _privateBuffer;

		// Check that we have a valid packet
		if(comms->magic != MAGIC_NUMBER)
		{
			printf("BAD MAGIC NUMBER. EXPECTED %x GOT %x\n", MAGIC_NUMBER, comms->magic);
			return PACKET_BAD;
		}

		// Packet is valid. Now let's do the checksum
		unsigned char checksum = 0;

		unsigned int i;

		for(i=0; i<comms->dataSize; i++)
			checksum ^= comms->buffer[i];

		if(checksum != comms->checksum)
			return PACKET_CHECKSUM_BAD;
		else
		{
			memcpy(output, comms->buffer, comms->dataSize);//copy the result to output//
			return PACKET_OK;
		}
	}
	else
		return result;//if not , just return PACKET_INCOMPLETE 
}

int serialize(char *buffer, void *dataStructure, size_t size)//takes any kind of datastructure via void* and empty buffer of [128]//
{
	TComms comms;

	// We use this to detect for malformed packets
	comms.magic = MAGIC_NUMBER;

	// Copy over the data structure
	memcpy(comms.buffer, dataStructure, size);//copy over data from the packet to the buffer in comms struct//

	// Now we take a checksum
	unsigned char checksum = 0;

	unsigned i;

	for(i=0; i<size; i++)
		checksum ^= comms.buffer[i];//checksum//

	comms.checksum = checksum;
	comms.dataSize = size;

	memcpy(buffer, &comms, sizeof(TComms));//copy from comms which includes the buffer which stores the packet into the empty buffer taken in by the parameter//

	return sizeof(TComms);
}

*/
