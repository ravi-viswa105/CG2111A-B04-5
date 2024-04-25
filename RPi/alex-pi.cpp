#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdint.h>
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
#include "prints.h"
using namespace std;
using namespace std::chrono;

#define PORT_NAME			"/dev/ttyACM0"
#define BAUD_RATE			B9600
#define DEFAULT_SPEED			40
#define DEFAULT_DELAY			150
#define LEFT_ARROW_KEY 			1073741904
#define RIGHT_ARROW_KEY 		1073741903

int exitFlag=0;
int commands_sent = 0;
int speed = DEFAULT_SPEED;
int delay_time = DEFAULT_DELAY;
bool clear_to_send_command = true;
// 1 for controlling with params, 2 for controlling with keypress
int keyboardMode = 1 ;
char previous_command = '0';
std::chrono::microseconds microsecond(1);
high_resolution_clock::time_point start = high_resolution_clock::now();
auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
sem_t _xmitSema;

//BAD MAGIC NUMBER HANDLING
//Mode 1 : NIL
//Mode 2 : 
//Mode 3 : No double commands
//Mode 4 : Commands every 1 second

void paramsControl() {
	printf_commands();
	char ch;
	printf("Mode: 1=param control, 2=keyboard control\n");
	scanf("%c", &ch);
        printf("send command : %c\n" , ch);
	sendCommand(ch);
}

void PressAndPress(){//mode 3
	int c;
	while((c=getch()) == ERR || (char(c) == previous_command && char(c) != ',' && char(c) != '.' && char(c) != 'v' && char(c) != 'u')){}
	//keeps polling for characters // characters must be not the same as the previous_commnd (cannot press w twice) // exception for '.' || 'v' || 'u' || ','
	char ch = c;
	
	refresh_screen();	
	if(ch == ','){
		decrease_speedw();
	}else if(ch == '.'){
		increase_speedw();
	}else{
        	printw("send command : %c\n" , ch);
        	refresh();
		sendCommand(ch);
		previous_command = ch;
	}
	commands_sent ++;
}

void TimedMovement(){//mode 4
	int c;
	while((c=getch()) == ERR){}
	char ch = c;
        refresh_screen();
	if(ch == ','){
		decrease_delay();
	}else if(ch == '.'){
		increase_delay();
	}else{
		printw("send command : %c\n" , ch);
		sendCommand(ch);
	}
        refresh();
	sleep(1);
	commands_sent++;
}

void keypressControl() {
	SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Keyboard Input", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 200, 200, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event event;
    bool quit = false;
    bool keyAlreadyPressed = false;
    bool stop = false;
    bool informed = false;

    while (!quit) {
        while (SDL_PollEvent(&event) && !stop) {
	    clear_to_send_command = true; //remove if want to test
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            else if (event.type == SDL_KEYUP){
		 if(keyAlreadyPressed){
		    auto key = event.key.keysym.sym;
		    if(key != LEFT_ARROW_KEY && key != RIGHT_ARROW_KEY && key != SDLK_u && key != SDLK_k && key != SDLK_v){
                    cout << "car stopped" << endl;
		    sendCommand('o');
		    }
		    keyAlreadyPressed = false;
		 }
            }
            else if (event.type == SDL_KEYDOWN && !keyAlreadyPressed){
		printf("clear to send command : %d\n" , clear_to_send_command);
	    	if(!clear_to_send_command){
			if(!informed){
				printf("cannot send command.Please release and press again");
				informed = true;
			}
	    	}else{
              	  keyAlreadyPressed = true;
		  informed = false;
                	switch(event.key.keysym.sym){
				case SDLK_1 :
				case SDLK_3 :
				case SDLK_4 :
                	        	quit = true;
                        		stop = true;
					keyboardMode = 1;
					break;
				case LEFT_ARROW_KEY ://self defined
					decrease_speedf();
					break;

				case RIGHT_ARROW_KEY : 
					increase_speedf();
					break;
				default :
					const char* string=  SDL_GetKeyName(event.key.keysym.sym);	
                        	        printf("send command : %c\n" , string[0]);
					sendCommand(string[0]);

               	 	}
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
	//printf("clear to send command : %d\n" , clear_to_send_command);
	TPacket helloPacket;

	helloPacket.packetType = PACKET_TYPE_HELLO;
	sendPacket(&helloPacket);

	//printf("clear to send command : %d\n" , clear_to_send_command);
	while(!exitFlag)
	{
		if (keyboardMode == 1) {
			paramsControl();
		} else if (keyboardMode == 2) {
			keypressControl();
		}else if(keyboardMode == 3){
			PressAndPress();
		}else if(keyboardMode == 4){
			TimedMovement();
		}
	}

	printf("Closing connection to Arduino.\n");
	endSerial();
}

