#include <stdarg.h>
#include <math.h>
#include "packet.h"
#include "constants.h"

#include <serialize.h>
#include <avr/io.h>
#include <avr/interrupt.h>

volatile TDirection dir;
bool preventCollision = false;

unsigned long red_freq;
unsigned long green_freq;
unsigned long blue_freq;
unsigned long linearactuatordist = 0;

#define PI 3.141592654
#define ALEX_LENGTH 12     
#define ALEX_BREADTH 13.5  
// min angle 33 deg
// Number of ticks per revolution from the
// wheel encoder.

#define COUNTS_PER_REV 4

// Wheel circumference in cm.
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

void enablePullups() { // pins 18 and 19 are PORTD pins 3 and 2 respectively
  DDRD &= ~(1<<2 | 1<<3); // set them as inputs
  PORTD |= 1<<2 | 1<<3; // set them both to high
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

// external interrupt ISRs
ISR(INT2_vect) {
  leftISR();
}

ISR(INT3_vect) {
  rightISR();
}


void setupEINT() {
  EIMSK |= (1<<3 | 1<<2); // enable INT2 and INT3
  EICRA |= 0b10100000; // set them both as falling-edge
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
  cli();
  setupEINT();
  setupSerial();
  startSerial();
  enablePullups();
  initializeState();
  setupUltrasonic();
  setup_colour_sensor();
 // InitTimer0();
  //StartTimer0();
  //DDRD |= 0b00000011;
  //ledcount = 0;
  sei();
}

int counter = 0;
void loop() {
  //preventCollision = true;
  if(preventCollision){
    int temp = getUltrasonicDistance();
    //Serial.println(temp);
    if(temp < 15 && temp > 10) {
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
