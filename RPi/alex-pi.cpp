#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdint.h>
#include <ncurses.h>
#include <termios.h>
#include <iostream>
#include <SDL2/SDL.h>
#include "packet.h"
#include "serial.h"
#include "serialize.h"
#include "constants.h"
#include "usart.h"
#include "colour.h"
using namespace std;


#define PORT_NAME			"/dev/ttyACM4"
#define BAUD_RATE			B9600
#define DEFAULT_SPEED			100
#define LEFT_ARROW_KEY 			1073741904
#define RIGHT_ARROW_KEY 		1073741903

int exitFlag=0;
int speed = DEFAULT_SPEED;
// 1 for controlling with params, 2 for controlling with keypress
int keyboardMode=1;
char previous_command = '0';
sem_t _xmitSema;


void paramsControl() {
	char ch;
	printf("Command (w=forward, s=reverse, a=turn left, d=turn right, o=stop, c=clear stats, g=get stats v= colour q=exit)\n");
	printf("Mode: 1=param control, 2=keyboard control\n");
	scanf("%c", &ch);
        printf("send command : %c\n" , ch);
	sendCommand(ch);
}

void PressAndPress(){
	printf("Command (w=forward, s=reverse, a=turn left, d=turn right, o=stop, c=clear stats, g=get stats v= colour q=exit)\n");
	refresh();
	int c;
	while((c=getch()) == ERR || char(c) == previous_command){}
	char ch = c;
        printw("Character input: %c\n", ch);
        refresh();
	sendCommand(ch);
	previous_command = ch;
}

void TimedMovement(){
	printf("Command (w=forward, s=reverse, a=turn left, d=turn right, o=stop, c=clear stats, g=get stats v= colour q=exit)\n");
	refresh();
	int c;
	while((c=getch()) == ERR){}
	char ch = c;
        printw("Character input: %c\n", ch);
        refresh();
	sendCommand(ch);
	usleep(1000000);
}

void keypressControl() {
	SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Keyboard Input", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event event;
    bool quit = false;
    bool keyAlreadyPressed = false;
    bool stop = false;

    while (!quit) {
        while (SDL_PollEvent(&event) && !stop) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            else if (event.type == SDL_KEYUP){
		    auto key = event.key.keysym.sym;
		    if(key != LEFT_ARROW_KEY && key != RIGHT_ARROW_KEY && key != SDLK_u && key != SDLK_k && key != SDLK_v){
                    cout << "car stopped" << endl;
		    sendCommand('o');
		    }
		    keyAlreadyPressed = false;
            }
            else if (event.type == SDL_KEYDOWN && !keyAlreadyPressed) {
                keyAlreadyPressed = true;
                switch(event.key.keysym.sym){
			case SDLK_1 :
                        	quit = true;
                        	stop = true;
				keyboardMode = 1;
				break;
			case LEFT_ARROW_KEY ://self defined
				speed -=5;
				printf("speed is decreased to %d\n" , speed);
				break;

			case RIGHT_ARROW_KEY : 
				if(speed+5 <= 100){
					speed += 5;
					printf("speed is increased to %d\n" , speed);
				}else{
					speed = 100;
					printf("speed is maxed at 100\n");
				}
				break;
			default :
				const char* string=  SDL_GetKeyName(event.key.keysym.sym);	
                                printf("send command : %c\n" , string[0]);
				sendCommand(string[0]);

                }
	    }
        }
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


int main()
{
	// Connect to the Arduino
	startSerial(PORT_NAME, BAUD_RATE, 8, 'N', 1, 5);

	// Sleep for two seconds
	printf("WAITING TWO SECONDS FOR ARDUINO TO REBOOT\n");
	sleep(2);
	printf("DONE\n");

	// Spawn receiver thread
	pthread_t recv;

	pthread_create(&recv, NULL, receiveThread, NULL);

	// Send a hello packet
	TPacket helloPacket;

	helloPacket.packetType = PACKET_TYPE_HELLO;
	sendPacket(&helloPacket);

	while(!exitFlag)
	{
		if (keyboardMode == 1) {
			paramsControl();
		} else if (keyboardMode == 2) {
			keypressControl();
		}else if(keyboardMode == 3){
			TimedMovement();
		}else if(keyboardMode == 4){
			PressAndPress();
		}
	}

	printf("Closing connection to Arduino.\n");
	endSerial();
}

