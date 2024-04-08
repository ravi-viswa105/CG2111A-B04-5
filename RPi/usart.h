
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdint.h>
#include "packet.h"
#include "serial.h"
#include "serialize.h"
#include "constants.h"

#include <ncurses.h>

extern int keyboardMode;
extern int exitFlag;

void handleError(TResult error);
void handleStatus(TPacket *packet);
void handleResponse(TPacket *packet);
void handleErrorResponse(TPacket *packet);
void handleMessage(TPacket *packet);
void handlePacket(TPacket *packet);
void sendPacket(TPacket *packet);
void *receiveThread(void *p);
void getParams(TPacket *commandPacket);
void sendCommand(char command);

void flushInput();
