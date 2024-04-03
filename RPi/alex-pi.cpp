#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdint.h>
#include "packet.h"
#include "serial.h"
#include "serialize.h"
#include "constants.h"
#include "usart.h"
#include <ncurses.h>
#include <termios.h>
#include <iostream>
#include <immintrin.h>
#include <SDL2/SDL.h>
using namespace std;


#define PORT_NAME			"/dev/ttyACM0"
#define BAUD_RATE			B9600

int exitFlag=0;
// 1 for controlling with params, 2 for controlling with keypress
int keyboardMode=1;
sem_t _xmitSema;


void paramsControl() {
	char ch;
	printf("Command (w=forward, s=reverse, a=turn left, d=turn right, o=stop, c=clear stats, g=get stats q=exit)\n");
	printf("Mode: 1=param control, 2=keyboard control\n");
	scanf("%c", &ch);
        printf("send command : %c\n" , ch);
	sendCommand(ch);
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
            else if (event.type == SDL_KEYUP) {
                cout << "car stopped" << endl;
		    keyAlreadyPressed = false;
		    sendCommand('o');
            }
            else if (event.type == SDL_KEYDOWN && !keyAlreadyPressed) {
                keyAlreadyPressed = true;
                switch(event.key.keysym.sym){
			case SDLK_1 :
                        	quit = true;
                        	stop = true;
				keyboardMode = 1;
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
		}
	}

	printf("Closing connection to Arduino.\n");
	endSerial();
}

/*
case SDLK_w : 
	cout << "car moving forward" << endl;
	break;
case SDLK_a :
        cout << "car turning left" << endl;
        break;
case SDLK_s :
        cout << "car reversing" << endl;
        break;
case SDLK_d :
        cout << "car turning right" << endl;
        break;
*/
