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


void handleColor(TPacket *packet);
float getPercentDiff(uint32_t a, uint32_t b);
void handleDistance(TPacket *packet);
