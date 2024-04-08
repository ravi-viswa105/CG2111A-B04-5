#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <stdint.h>

#define MAX_STR_LEN   14
// This packet is 1 + 1 + 2 + 4 * 4 = 20 bytes
typedef struct
{
	char packetType;
	char command;
	char dummy[2]; // Padding to make up 4 bytes
	// char data[MAX_STR_LEN]; // String data
	// uint32_t params[16];
	uint32_t params[3];
} TPacket;

typedef struct 
{
	char packetType;
	char command;
	char dummy[2]; // Padding to make up 4 bytes
	char data[MAX_STR_LEN];
} TMessage;


#endif
