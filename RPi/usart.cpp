#include "usart.h"

void handleError(TResult error)
{
	switch(error)
	{
		case PACKET_BAD:
			printf("ERROR: Bad Magic Number\n");
			break;

		case PACKET_CHECKSUM_BAD:
			printf("ERROR: Bad checksum\n");
			break;

		default:
			printf("ERROR: UNKNOWN ERROR\n");
	}
}

void handleStatus(TPacket *packet)
{
	printf("\n ------- ALEX STATUS REPORT ------- \n\n");
	printf("Left Forward Ticks:\t\t%d\n", packet->params[0]);
	printf("Right Forward Ticks:\t\t%d\n", packet->params[1]);
	printf("Left Reverse Ticks:\t\t%d\n", packet->params[2]);
	printf("Right Reverse Ticks:\t\t%d\n", packet->params[3]);
	printf("Left Forward Ticks Turns:\t%d\n", packet->params[4]);
	printf("Right Forward Ticks Turns:\t%d\n", packet->params[5]);
	printf("Left Reverse Ticks Turns:\t%d\n", packet->params[6]);
	printf("Right Reverse Ticks Turns:\t%d\n", packet->params[7]);
	printf("Forward Distance:\t\t%d\n", packet->params[8]);
	printf("Reverse Distance:\t\t%d\n", packet->params[9]);
	printf("\n---------------------------------------\n\n");
}

void handleResponse(TPacket *packet)
{
	// The response code is stored in command
	switch(packet->command)
	{
		case RESP_OK:
			
			if(keyboardMode == 3 || keyboardMode == 4){
				printw("Command OK\n");
			}else{
				printf("Command OK\n");
			}
			clear_to_send_command = true;
		break;

		case RESP_STATUS:
			handleStatus(packet);
		break;

		case RESP_COLOR:
			handleColor(packet);
		break;

		case RESP_DIST:
			handleDistance(packet);
			break;


		default:
			printf("Arduino is confused\n");
	}
}

void handleErrorResponse(TPacket *packet)
{
	// The error code is returned in command
	switch(packet->command)
	{
		case RESP_BAD_PACKET:
			printf("Arduino received bad magic number\n");
		break;

		case RESP_BAD_CHECKSUM:
			printf("Arduino received bad checksum\n");
		break;

		case RESP_BAD_COMMAND:
			printf("Arduino received bad command\n");
		break;

		case RESP_BAD_RESPONSE:
			printf("Arduino received unexpected response\n");
		break;

		default:
			printf("Arduino reports a weird error\n");
	}
}

void handleMessage(TPacket *packet)
{
	printf("Message from Alex: %s\n", packet->data);
}

void handlePacket(TPacket *packet)
{
	switch(packet->packetType)
	{
		case PACKET_TYPE_COMMAND_PARAM:
				// Only we send command packets, so ignore
			break;
		case PACKET_TYPE_COMMAND_KEYBOARD:
				// Only we send command packets, so ignore
			break;

		case PACKET_TYPE_COMMAND_TIME :
			break;
				// Only we send command packets, so ignore
		case PACKET_TYPE_RESPONSE:
				handleResponse(packet);
			break;

		case PACKET_TYPE_ERROR:
				handleErrorResponse(packet);
			break;

		case PACKET_TYPE_MESSAGE:
				handleMessage(packet);
			break;
	}
}

void sendPacket(TPacket *packet)
{	
	/*if(packet->packetType == PACKET_TYPE_COMMAND_KEYBOARD || packet->packetType == PACKET_TYPE_COMMAND_TIME || packet->packetType == PACKET_TYPE_COMMAND_PARAM){
		clear_to_send_command = false;
	}*/
	char buffer[PACKET_SIZE];
	int len = serialize(buffer, packet, sizeof(TPacket));

	serialWrite(buffer, len);
}

void *receiveThread(void *p)
{
	char buffer[PACKET_SIZE];
	int len;
	TPacket packet;
	TResult result;
	int counter=0;

	while(1)
	{
		len = serialRead(buffer);
		counter+=len;
		if(len > 0)
		{
			result = deserialize(buffer, len, &packet);

			if(result == PACKET_OK)
			{
				counter=0;
				handlePacket(&packet);
			}
			else 
				if(result != PACKET_INCOMPLETE)
				{
					printf("PACKET ERROR\n");
					handleError(result);
				}
		}
	}
}

void getParams(TPacket *commandPacket)
{
   switch(keyboardMode){

	case 1:

		printf("Enter distance/angle in cm/degrees (e.g. 50) and power in %% (e.g. 75) separated by space.\n");
		printf("E.g. 50 75 means go at 50 cm at 75%% power for forward/backward, or 50 degrees left or right turn at 75%%  power\n");
		scanf("%d %d", &commandPacket->params[0], &commandPacket->params[1]);
		break;
	case 2 : 
	case 3 :	
	    	commandPacket->params[0] = speed;
		break;
	case 4:
		commandPacket->params[0] = delay_time;
		break;
	default:
		break;
   }
}

void sendCommand(char command)
{
	TPacket commandPacket;

	if (keyboardMode == 1) {
        	commandPacket.packetType = PACKET_TYPE_COMMAND_PARAM;
    	}else if(keyboardMode == 2 || keyboardMode == 3){
        	commandPacket.packetType = PACKET_TYPE_COMMAND_KEYBOARD;
    	}else{
		commandPacket.packetType = PACKET_TYPE_COMMAND_TIME;
    	}

	switch(command)
	{
        // Changing the movement mode
        case '1':
	    if(keyboardMode == 3 || keyboardMode == 4){
	    	endwin();
	    }
	    sleep(1);
            keyboardMode = 1;
	    printf("Switched to params mode\n");
            break;

        case '2':
	    if(keyboardMode == 3 || keyboardMode == 4){
	    	endwin();
	    }
	    sleep(1);
            keyboardMode = 2;
	    printf("Switched to Key Hold mode\n");
            break;

        case '3':
	    if(keyboardMode == 1 || keyboardMode == 2){
	    	initscr();
    	    	cbreak(); // Disable line buffering
            	noecho(); // Don't echo input characters
    	    	nodelay(stdscr, TRUE); // Set non-blocking mod
	    }

            keyboardMode = 3;
	    printw("Switched to Key Press mode\n");
	    refresh();
            break;
	
        case '4':
	    if(keyboardMode == 1 || keyboardMode == 2){
	    	initscr();
    	    	cbreak(); // Disable line buffering
            	noecho(); // Don't echo input characters
    	    	nodelay(stdscr, TRUE); // Set non-blocking mod
	    }
            keyboardMode = 4;
	    printw("Switched to Small Adjustment mode\n");
	    refresh();
            break;

               // Movement
		case 'w':
		case 'W':
			getParams(&commandPacket);
			commandPacket.command = COMMAND_FORWARD;
			sendPacket(&commandPacket);
			break;

		case 's':
		case 'S':
			getParams(&commandPacket);
			commandPacket.command = COMMAND_REVERSE;
			sendPacket(&commandPacket);
			break;

		case 'a':
		case 'A':
			getParams(&commandPacket);
			commandPacket.command = COMMAND_TURN_LEFT;
			sendPacket(&commandPacket);
			break;

		case 'd':
		case 'D':
			getParams(&commandPacket);
			commandPacket.command = COMMAND_TURN_RIGHT;
			sendPacket(&commandPacket);
			break;

		case 'o':
		case 'O':
			commandPacket.command = COMMAND_STOP;
			sendPacket(&commandPacket);
			break;

               // Stats
		case 'k':
		case 'K':
			commandPacket.command = COMMAND_CLEAR_STATS;
			commandPacket.params[0] = 0;
			sendPacket(&commandPacket);
			break;

		case 'g':
		case 'G':
			commandPacket.command = COMMAND_GET_STATS;
			sendPacket(&commandPacket);
			break;

		case 'v':
		case 'V':
			printf("GET COLOR\n");
			commandPacket.command = COMMAND_COLOR;
			sendPacket(&commandPacket);
			break;
		case 'u':
		case 'U':
			printf("GET DISTANCE\n");
			commandPacket.command = COMMAND_DIST;
			sendPacket(&commandPacket);
			break;

		case 'l':
		case 'L':
			printf("TURNING ON LCD\n");
			commandPacket.command = COMMAND_LCD;
			sendPacket(&commandPacket);
			break;

		case 'c':
		case 'C':
			clear();
			printw("Command (w=forward, s=reverse, a=turn left, d=turn right, o=stop, c=clear stats, g=get stats v= colour q=exit)\n");
			refresh();
			break;

        // Others
		case 'q':
		case 'Q':
			if(keyboardMode == 3 || keyboardMode == 4){
				endwin();
			}
			exitFlag=1;
			break;

		default:
			printf("Bad command\n");

	}
}
