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

#define PORT_NAME			"/dev/ttyACM0"
#define BAUD_RATE			B9600

int exitFlag=0;
// 1 for controlling with params, 2 for controlling with keypress
int keyboardMode=1;
bool movementFlag=0;
int counter_hit = 0;
int counter_nohit = 0;

sem_t _xmitSema;

int kbhit(void){
	int ch = getch();

	if(ch != ERR){
		ungetch(ch);
		return 1;
	}else{
		return 0;
	}
}

/*
	char c = getcharacter();

	if (!movementFlag) {
		// If it is not moving and a movement command is sent
		if (c == 'w' || c == 'a' || c == 's' || c == 'd') { 
			printf("starting\n\r");
			movementFlag = 1;
		}

		sendCommand(c);
	} else if (movementFlag && (c == -1)) { 
		// If robot is moving and key is released
		printf("stopping\n\r");
		movementFlag = 0;
		sendCommand('o');
}*/
void paramsControl() {
	char ch;
	printf("Command (w=forward, s=reverse, a=turn left, d=turn right, o=stop, c=clear stats, g=get stats q=exit)\n");
	printf("Mode: 1=param control, 2=keyboard control\n");
	scanf("%c", &ch);

	// Purge extraneous characters from input stream
	fflush(stdin);//CHECK
	sendCommand(ch);
}

void keypressControl() {
	if(kbhit()){
		counter_hit ++;
		if(!movementFlag && counter_hit >= 20){
			char c = getch();
                        sendCommand(c);
			movementFlag = true;
			counter_nohit = 0;
		}
		refresh();
	}else {
		counter_nohit ++;
		if(movementFlag && counter_nohit >= 20){
			sendCommand('o');
			counter_hit = 0;
			movementFlag = false;
		}
		refresh();
	}
	usleep(5000);
	fflush(stdin);
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
	initscr();
	cbreak();
	noecho();
	nodelay(stdscr , TRUE);
	scrollok(stdscr , TRUE);

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
