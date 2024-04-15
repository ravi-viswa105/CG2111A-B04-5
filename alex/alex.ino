#include <stdarg.h>
#include <math.h>
#include "packet.h"
#include "constants.h"

#include <serialize.h>
#include <avr/io.h>
#include <avr/interrupt.h>

volatile int count;
volatile TDirection dir;
bool preventCollision = false;

#define PI 3.141592654
#define ALEX_LENGTH 12     //to change
#define ALEX_BREADTH 13.5  //to change
// min angle 33 deg
// Number of ticks per revolution from the
// wheel encoder.

#define COUNTS_PER_REV 4

// Wheel circumference in cm.
// We will use this to calculate forward/backward distance traveled
// by taking revs * WHEEL_CIRC

#define WHEEL_CIRC 20.73  //cm

float alexDiagonal = sqrt((ALEX_LENGTH * ALEX_LENGTH) + (ALEX_BREADTH * ALEX_BREADTH));
float alexCirc = (PI * alexDiagonal);

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
 * Setup and start codes for external interrupts and 
 * pullup resistors.
 * 
 */
// Enable pull up resistors on pins 18 and 19
void enablePullups() {
  // Use bare-metal to enable the pull-up resistors on pins
  DDRD &= 0b11110011;
  PORTD |= 0b00001100;
  // 19 and 18. These are pins PD2 and PD3 respectively.
  // We set bits 2 and 3 in DDRD to 0 to make them inputs.
}

// Functions to be called by INT2 and INT3 ISRs.
void leftISR() {
  if (dir == FORWARD) {
    leftForwardTicks++;
    forwardDist = (unsigned long)((float)leftForwardTicks / COUNTS_PER_REV * WHEEL_CIRC);
  } else if (dir == BACKWARD) {
    leftReverseTicks++;
    reverseDist = (unsigned long)((float)leftReverseTicks / COUNTS_PER_REV * WHEEL_CIRC);
  } else if (dir == LEFT) {
    leftReverseTurns++;
  } else if (dir == RIGHT) {
    leftForwardTurns++;
  }
}

void rightISR() {
  if (dir == FORWARD) {
    rightForwardTicks++;
  } else if (dir == BACKWARD) {
    rightReverseTicks++;
  } else if (dir == LEFT) {
    rightForwardTurns++;
  } else if (dir == RIGHT) {
    rightReverseTurns++;
  }
}

// Implement the external interrupt ISRs below.
// INT3 ISR should call leftISR while INT2 ISR
// should call rightISR.
ISR(INT2_vect) {
  leftISR();
}

ISR(INT3_vect) {
  rightISR();
}


// Set up the external interrupt pins INT2 and INT3
// for falling edge triggered. Use bare-metal.
void setupEINT() {
  // Use bare-metal to configure pins 18 and 19 to be
  // falling edge triggered. Remember to enable
  // the INT2 and INT3 interrupts.
  // Hint: Check pages 110 and 111 in the ATmega2560 Datasheet.
  EIMSK = 0b00001100;
  EICRA = 0b10100000;
}


/*
 * Alex's setup and run codes
 * 
 */

// Clears all our counters
void clearCounters() {
  leftForwardTicks = 0;
  rightForwardTicks = 0;
  leftReverseTicks = 0;
  rightReverseTicks = 0;

  leftForwardTurns = 0;
  rightForwardTurns = 0;
  leftReverseTurns = 0;
  rightReverseTurns = 0;

  forwardDist = 0;
  reverseDist = 0;

  deltaTicks = 0;  //should be removed soon?
  targetTicks = 0;
  deltaDist = 0;
  newDist = 0;
}

// Clears one particular counter
void clearOneCounter(int which) {
  clearCounters();  //dk
  sendOK();
  return;

  switch (which) {
    case 0:
      leftForwardTicks = 0;
      break;
    case 1:
      rightForwardTicks = 0;
      break;
    case 2:
      leftReverseTicks = 0;
      break;
    case 3:
      rightReverseTicks = 0;
      break;
    case 4:
      leftForwardTurns = 0;
      break;
    case 5:
      rightForwardTurns = 0;
      break;
    case 6:
      leftReverseTurns = 0;
      break;
    case 7:
      rightReverseTurns = 0;
      break;
    case 8:
      forwardDist = 0;
      break;
    case 9:
      reverseDist = 0;
      break;
  }
}
// Intialize Alex's internal states

void initializeState() {
  clearCounters();
}


void setup() {
  // put your setup code here, to run once:
  cli();
  setupEINT();
  setupSerial();
  startSerial();
  enablePullups();
  initializeState();
  setupUltrasonic();
  setup_colour_sensor();

  //InitTimer0();
  //StartTimer0();
  //DDRD |= 0b00000011;
  //count = 0;
  sei();
}

void loop() {
  //preventCollision = true;
  if(preventCollision){
    int temp = getUltrasonicDistance();
    //Serial.println(temp);
    if(temp < 8 && temp > 5) {
      stop();
      sendMessage("auto stopped");
    }
  } 
  if (deltaDist > 0) {
    if (dir == FORWARD) {
      if (forwardDist > newDist) {
        deltaDist = 0;
        newDist = 0;
        stop();
        delay(1000);
      }
    } else if (dir == BACKWARD) {
      if (reverseDist > newDist) {
        deltaDist = 0;
        newDist = 0;
        stop();
        delay(1000);
      }
    } else if(dir == STOP){
      deltaDist = 0;
      newDist = 0;
      stop();
      delay(1000);
    }
  }

  if (deltaTicks > 0) {
    if (dir == LEFT) {
      //unsigned long current_ticks = max(rightForwardTurns,leftReverseTurns);
      if (leftReverseTurns >= targetTicks) {
        deltaTicks = 0;
        targetTicks = 0;
        //rightForwardTurns = current_ticks;
        //leftReverseTurns = current_ticks;
        stop();
        delay(1000);
      }
    } else if (dir == RIGHT) {
      //unsigned long current_ticks = max(rightReverseTurns,leftForwardTurns);
      if (rightReverseTurns >= targetTicks) {
        deltaTicks = 0;
        targetTicks = 0;
        //rightReverseTurns = current_ticks;
        //leftForwardTurns = current_ticks;
        stop();
        delay(1000);
      }
    } else if(dir == STOP){
      deltaTicks = 0;
      targetTicks = 0;
      stop();
      delay(1000);
    }
  }

  TPacket recvPacket;  // This holds commands from the Pi

  TResult result = readPacket(&recvPacket);

  if (result == PACKET_OK)
    handlePacket(&recvPacket);
  else if (result == PACKET_BAD) {
    sendBadPacket();
  } else if (result == PACKET_CHECKSUM_BAD) {
    sendBadChecksum();
  }
}
