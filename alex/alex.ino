#include <serialize.h>
#include <stdarg.h>
#include <math.h>
#include "packet.h"
#include "constants.h"

volatile TDirection dir;

#define PI 3.141592654
#define ALEX_LENGTH 12//to change
#define ALEX_BREADTH 13.5//to change
#define alexDiagonal sqrt((ALEX_LENGTH * ALEX_LENGTH) + (ALEX_BREADTH * ALEX_BREADTH))
#define alexCirc (PI * alexDiagonal)
//min angle 33 deg
// Number of ticks per revolution from the 
// wheel encoder.

#define COUNTS_PER_REV      4

// Wheel circumference in cm.
// We will use this to calculate forward/backward distance traveled 
// by taking revs * WHEEL_CIRC

#define WHEEL_CIRC          20.73 //cm

// Store the ticks from Alex's left and
// right encoders.
volatile unsigned long leftForwardTicks; 
volatile unsigned long rightForwardTicks;
volatile unsigned long leftReverseTicks; 
volatile unsigned long rightReverseTicks;

volatile unsigned long leftForwardTurns; 
volatile unsigned long rightForwardTurns;
volatile unsigned long leftReverseTurns; 
volatile unsigned long rightReverseTurns;

// Forward and backward distance traveled
volatile unsigned long forwardDist;
volatile unsigned long reverseDist;
unsigned long deltaDist;
unsigned long newDist;

unsigned long deltaTicks;
unsigned long targetTicks;
/*
 * 
 * Alex Communication Routines.
 * 
 */
 
TResult readPacket(TPacket *packet)
{
    // Reads in data from the serial port and
    // deserializes it.Returns deserialized
    // data in "packet".
    
    char buffer[PACKET_SIZE];
    int len;

    len = readSerial(buffer);

    if(len == 0)
      return PACKET_INCOMPLETE;
    else
      return deserialize(buffer, len, packet);
    
}

void sendStatus()
{
  TPacket statusPacket;
  statusPacket.packetType = PACKET_TYPE_RESPONSE;
  statusPacket.command = RESP_STATUS;
  statusPacket.params[0] = leftForwardTicks;
  statusPacket.params[1] = rightForwardTicks;
  statusPacket.params[2] = leftReverseTicks;
  statusPacket.params[3] = rightReverseTicks;
  statusPacket.params[4] = leftForwardTurns;
  statusPacket.params[5] = rightForwardTurns;
  statusPacket.params[6] = leftReverseTurns;
  statusPacket.params[7] = rightReverseTurns;
  statusPacket.params[8] = forwardDist;
  statusPacket.params[9] = reverseDist;
  statusPacket.params[10] = deltaTicks;
  statusPacket.params[11] = targetTicks;
  sendResponse(&statusPacket);
}

void sendMessage(const char *message)
{
  // Sends text messages back to the Pi. Useful
  // for debugging.
 
  TPacket messagePacket;
  messagePacket.packetType=PACKET_TYPE_MESSAGE;
  strncpy(messagePacket.data, message, MAX_STR_LEN);
  sendResponse(&messagePacket);
}

void dbprintf(char *format, ...) {
  va_list args;
  char buffer[128];
  va_start(args, format);
  vsprintf(buffer, format, args);
  sendMessage(buffer);
} 

void sendBadPacket()
{
  // Tell the Pi that it sent us a packet with a bad
  // magic number.
  
  TPacket badPacket;
  badPacket.packetType = PACKET_TYPE_ERROR;
  badPacket.command = RESP_BAD_PACKET;
  sendResponse(&badPacket);
  
}

void sendBadChecksum()
{
  // Tell the Pi that it sent us a packet with a bad
  // checksum.
  
  TPacket badChecksum;
  badChecksum.packetType = PACKET_TYPE_ERROR;
  badChecksum.command = RESP_BAD_CHECKSUM;
  sendResponse(&badChecksum);  
}

void sendBadCommand()
{
  // Tell the Pi that we don't understand its
  // command sent to us.
  
  TPacket badCommand;
  badCommand.packetType=PACKET_TYPE_ERROR;
  badCommand.command=RESP_BAD_COMMAND;
  sendResponse(&badCommand);
}

void sendBadResponse()
{
  TPacket badResponse;
  badResponse.packetType = PACKET_TYPE_ERROR;
  badResponse.command = RESP_BAD_RESPONSE;
  sendResponse(&badResponse);
}

void sendOK()
{
  TPacket okPacket;
  okPacket.packetType = PACKET_TYPE_RESPONSE;
  okPacket.command = RESP_OK;
  sendResponse(&okPacket);  
}

void sendResponse(TPacket *packet)
{
  // Takes a packet, serializes it then sends it out
  // over the serial port.
  char buffer[PACKET_SIZE];
  int len;

  len = serialize(buffer, packet, sizeof(TPacket));
  writeSerial(buffer, len);
}


/*
 * Setup and start codes for external interrupts and 
 * pullup resistors.
 * 
 */
// Enable pull up resistors on pins 18 and 19
void enablePullups()
{
  // Use bare-metal to enable the pull-up resistors on pins
  DDRD &= 0b11110011;
  PORTD |= 0b00001100;
  // 19 and 18. These are pins PD2 and PD3 respectively.
  // We set bits 2 and 3 in DDRD to 0 to make them inputs. 
  
}

// Functions to be called by INT2 and INT3 ISRs.
void leftISR()
{
  if(dir == FORWARD){
    leftForwardTicks ++;
    forwardDist = (unsigned long) ((float) leftForwardTicks / COUNTS_PER_REV * WHEEL_CIRC);
  }else if(dir == BACKWARD){
    leftReverseTicks ++;
    reverseDist = (unsigned long) ((float) leftReverseTicks / COUNTS_PER_REV * WHEEL_CIRC);
  }else if(dir == LEFT){
    leftReverseTurns ++;
  }else if(dir == RIGHT){
    leftForwardTurns ++;
  }
}

void rightISR()
{
   if(dir == FORWARD){
    rightForwardTicks ++;
  }else if(dir == BACKWARD){
    rightReverseTicks ++;
  }else if(dir == LEFT){
    rightForwardTurns ++;
  }else if(dir == RIGHT){
    rightReverseTurns ++;
  }
}

ISR(INT2_vect){
  leftISR();
}

ISR(INT3_vect){
  rightISR();
}



// Set up the external interrupt pins INT2 and INT3
// for falling edge triggered. Use bare-metal.
void setupEINT()
{
  // Use bare-metal to configure pins 18 and 19 to be
  // falling edge triggered. Remember to enable
  // the INT2 and INT3 interrupts.
  // Hint: Check pages 110 and 111 in the ATmega2560 Datasheet.
  EIMSK = 0b00001100;
  EICRA = 0b10100000;
}

// Implement the external interrupt ISRs below.
// INT3 ISR should call leftISR while INT2 ISR
// should call rightISR.


// Implement INT2 and INT3 ISRs above.

/*
 * Setup and start codes for serial communications
 * 
 */
// Set up the serial connection. For now we are using 
// Arduino Wiring, you will replace this later
// with bare-metal code.
void setupSerial()
{
  // To replace later with bare-metal.
  Serial.begin(9600);
  // Change Serial to Serial2/Serial3/Serial4 in later labs when using the other UARTs
}

// Start the serial connection. For now we are using
// Arduino wiring and this function is empty. We will
// replace this later with bare-metal code.

void startSerial()
{
  // Empty for now. To be replaced with bare-metal code
  // later on.
  
}

// Read the serial port. Returns the read character in
// ch if available. Also returns TRUE if ch is valid. 
// This will be replaced later with bare-metal code.

int readSerial(char *buffer)
{

  int count=0;

  // Change Serial to Serial2/Serial3/Serial4 in later labs when using other UARTs

  while(Serial.available())
    buffer[count++] = Serial.read();

  return count;
}

// Write to the serial port. Replaced later with
// bare-metal code

void writeSerial(const char *buffer, int len)
{
  Serial.write(buffer, len);
  // Change Serial to Serial2/Serial3/Serial4 in later labs when using other UARTs
}

/*
 * Alex's setup and run codes
 * 
 */

// Clears all our counters
void clearCounters()
{
  leftForwardTicks = 0 ; 
  rightForwardTicks = 0;
  leftReverseTicks = 0; 
  rightReverseTicks = 0;

  leftForwardTurns = 0; 
  rightForwardTurns = 0;
  leftReverseTurns = 0; 
  rightReverseTurns = 0;
 
  forwardDist=0;
  reverseDist=0; 

  deltaTicks = 0;//should be removed soon?
  targetTicks = 0;
  deltaDist = 0;
  newDist = 0;
}

// Clears one particular counter
void clearOneCounter(int which)
{
  clearCounters();//dk
  sendOK();
  return;

  switch(which){
    case 0 :
       leftForwardTicks = 0 ; 
       break;
    case 1 :
       rightForwardTicks = 0 ; 
       break;
    case 2 :
       leftReverseTicks = 0 ; 
       break;
    case 3 :
       rightReverseTicks = 0 ; 
       break;
    case 4 :
       leftForwardTurns = 0 ; 
       break;
    case 5 :
       rightForwardTurns = 0 ; 
       break;
    case 6 :
       leftReverseTurns = 0 ; 
       break;
    case 7 :
       rightReverseTurns = 0 ; 
       break;
    case 8 :
       forwardDist = 0 ; 
       break;
    case 9 :
       reverseDist = 0 ; 
       break;
  }
}
// Intialize Alex's internal states

void initializeState()
{
  clearCounters();
}

void handleCommand(TPacket *command)
{
  switch(command->command)
  {
    // For movement commands, param[0] = distance, param[1] = speed.
    case COMMAND_FORWARD:
        sendOK();
        forward((double) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_REVERSE:
        sendOK();
        backward((double) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_TURN_LEFT:
        sendOK();
        left((double) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_TURN_RIGHT:
        sendOK();
        right((double) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_STOP:
        sendOK();
        stop();
      break;
    case COMMAND_GET_STATS:
        sendOK();
        sendStatus();
      break;
    case COMMAND_CLEAR_STATS:
        sendOK();
        clearCounters();
      break;        
    default:
      sendBadCommand();
  }
}

void waitForHello()
{
  int exit=0;

  while(!exit)
  {
    TPacket hello;
    TResult result;
    
    do
    {
      result = readPacket(&hello);
    } while (result == PACKET_INCOMPLETE);

    if(result == PACKET_OK)
    {
      if(hello.packetType == PACKET_TYPE_HELLO)
      {
        sendOK();
        exit=1;
      }
      else
        sendBadResponse();
    }
    else
      if(result == PACKET_BAD)
      {
        sendBadPacket();
      }
      else
        if(result == PACKET_CHECKSUM_BAD)
          sendBadChecksum();
  } // !exit
}

void setup() {
  // put your setup code here, to run once:

  cli();
  setupEINT();
  setupSerial();
  startSerial();
  enablePullups();
  initializeState();
  sei();
}

void handlePacket(TPacket *packet)
{
  switch(packet->packetType)
  {
    case PACKET_TYPE_COMMAND:
      handleCommand(packet);
      break;

    case PACKET_TYPE_RESPONSE:
      break;

    case PACKET_TYPE_ERROR:
      break;

    case PACKET_TYPE_MESSAGE:
      break;

    case PACKET_TYPE_HELLO:
      break;
  }
}

void loop() {

 if(deltaDist > 0){
    if(dir==FORWARD){
      if(forwardDist > newDist){
        deltaDist=0;
        newDist=0;
        stop();
        delay(1000);
      }
    }else if(dir == BACKWARD){
      if(reverseDist > newDist){
        deltaDist=0;
        newDist=0;
        stop();
        delay(1000);
      }
    }else{
      deltaDist=0;
      newDist=0;
      stop();
      delay(1000);
    }
  }  
  if(deltaTicks > 0){
    if(dir==LEFT){
      //unsigned long current_ticks = max(rightForwardTurns,leftReverseTurns);
      if(leftReverseTurns >= targetTicks){
        deltaTicks =0;
        targetTicks =0;
        //rightForwardTurns = current_ticks;
        //leftReverseTurns = current_ticks;
        stop();
        delay(1000);
      }
    }else if(dir == RIGHT){
      //unsigned long current_ticks = max(rightReverseTurns,leftForwardTurns);
      if(rightReverseTurns >= targetTicks){
        deltaTicks = 0;
        targetTicks = 0;
        //rightReverseTurns = current_ticks;
        //leftForwardTurns = current_ticks;
        stop();
        delay(1000);
      } 
    }else{
      deltaTicks=0;
      targetTicks=0;
      stop();
      delay(1000);
    }
  }  

  TPacket recvPacket; // This holds commands from the Pi

  TResult result = readPacket(&recvPacket);
  
  if(result == PACKET_OK)
    handlePacket(&recvPacket);
  else
    if(result == PACKET_BAD)
    {
      sendBadPacket();
    }
    else
      if(result == PACKET_CHECKSUM_BAD)
      {
        sendBadChecksum();
      } 


 findColour();
  
}
