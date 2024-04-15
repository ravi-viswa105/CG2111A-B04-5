#include <ncurses.h>
#include <termios.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <chrono>
#include "packet.h"
#include "serial.h"
#include "serialize.h"
#include "constants.h"
#include "usart.h"
#include "colour.h"
#include "time.h"
using namespace std;
using namespace std::chrono;
extern int speed;
extern int commands_sent;

void printw_commands();
void printf_commands();
void increase_speedw();
void decrease_speedw();
void increase_speedf();
void decrease_speedf();
void refresh_screen();
void print_colourf(uint32_t red , uint32_t green , uint32_t blue , float redGreenDiff , float blueGreenDiff);
void print_colourw(uint32_t red , uint32_t green , uint32_t blue , float redGreenDiff , float blueGreenDiff);

