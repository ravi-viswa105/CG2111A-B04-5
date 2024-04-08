#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <stdint.h>

#define MAX_STR_LEN   32
// This packet is 1 + 1 + 2 + 2 * 4 = 12 bytes
typedef struct
{
	char packetType;
	char command;
	char dummy[2]; // Padding to make up 4 bytes
	// char data[MAX_STR_LEN]; // String data
	// uint32_t params[16];
	uint32_t params[2];
} TPacket;

// This packet is 1 + 1 + 2 + 3 * 4 = 16 bytes
typedef struct 
{
	char packetType;
	char command;
	char dummy[2]; // Padding to make up 4 bytes
	uint32_t red;
	uint32_t green;
	uint32_t blue;
} TColour

#endif
