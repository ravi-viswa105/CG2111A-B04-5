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

#define PORT_NAME			"/dev/ttyACM0"
#define BAUD_RATE			B9600

int exitFlag=0;
// 1 for controlling with params, 2 for controlling with keypress
int keyboardMode=1;
int movementFlag=0;
sem_t _xmitSema;

void flushInput()
{
	char c;
	while((c = getchar()) != '\n' && c != EOF);
}

void paramsControl() {
	char ch;
	printf("Command (w=forward, s=reverse, a=turn left, d=turn right, o=stop, c=clear stats, g=get stats q=exit)\n");
	printf("Mode: 1=param control, 2=keyboard control\n");
	scanf("%c", &ch);

	// Purge extraneous characters from input stream
	flushInput();

	sendCommand(ch);
}

void keypressControl() {
	int c = getch();
	putchar(char(c));
	printf("\n\r");

	if (!movementFlag &&  (c == 'w' || c == 'a' || c == 's' || c == 'd')) {
		// If it is not moving and a movement command is sent
		printf("moving\n\r");
		movementFlag = 1;
		sendCommand((char)c);
	} else if (movementFlag && (c == ERR)) { 
		// If robot is moving and key is released
		printf("stopping\n\r");
		movementFlag = 0;
		sendCommand('o');
	} else if (c != ERR) {
		sendCommand((char)c);
	}
	sleep(1);
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

	// Set up terminal	
    initscr(); // Initialize the screen and sets up memory, but does not clear the screen
	nodelay(stdscr, TRUE); // make getch() non-blocking
	keypad(stdscr, TRUE);
	
	if (keyboardMode == 1) {
		nocbreak();
	} else {
		cbreak();
	}

	while(!exitFlag)
	{
		if (keyboardMode == 1) {
			paramsControl();
		} else if (keyboardMode == 2) {
			keypressControl();
		}
	}

    //echo(); // Re-enable echoing of keypresses
	endwin();

	printf("Closing connection to Arduino.\n");
	endSerial();
}
