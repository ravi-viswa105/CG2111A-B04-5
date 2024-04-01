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

#include <termios.h>

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

char getcharacter() {
	char buf = 0;
	struct termios old = {0};
	if (tcgetattr(0, &old) < 0)
		perror("tcsetattr()");
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old) < 0)
		perror("tcsetattr ICANON");
	if (read(0, &buf, 1) < 0)
		perror ("read()");
	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		perror ("tcsetattr ~ICANON");
	return (buf);
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
	}
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
