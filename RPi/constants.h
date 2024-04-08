
#ifndef __CONSTANTS_INC__
#define __CONSTANTS_INC__

/* 
 *  This file containts all the packet types, commands
 *  and status constants
 *  
 */

// Packet types
typedef enum
{
  PACKET_TYPE_COMMAND_PARAM = 0,
  PACKET_TYPE_COMMAND_KEYBOARD = 1,
  PACKET_TYPE_RESPONSE = 2,
  PACKET_TYPE_ERROR = 3,
  PACKET_TYPE_MESSAGE = 4,
  PACKET_TYPE_HELLO = 5
} TPacketType;

// Response types. This goes into the command field
typedef enum
{
  RESP_OK = 0,
  RESP_STATUS = 1,
  RESP_BAD_PACKET = 2,
  RESP_BAD_CHECKSUM = 3,
  RESP_BAD_COMMAND = 4,
  RESP_BAD_RESPONSE = 5, 
  RESP_COLOR = 6,
  RESP_DIST = 7,
  RESP_STATS = 8
} TResponseType;


// Commands
// For direction commands, param[0] = distance in cm to move
// param[1] = speed
typedef enum
{
  COMMAND_FORWARD = 0,
  COMMAND_REVERSE = 1,
  COMMAND_TURN_LEFT = 2,
  COMMAND_TURN_RIGHT = 3,
  COMMAND_STOP = 4,
  COMMAND_GET_STATS = 5,
  COMMAND_CLEAR_STATS = 6,
  COMMAND_COLOR = 7,
  COMMAND_DIST = 8
} TCommandType;
#endif

