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
	printf("\n ------- ALEX DISTANCE REPORT ------- \n\n");
	printf("Forward Distance:\t\t%d\n", packet->params[0]);
	printf("Reverse Distance:\t\t%d\n", packet->params[1]);
	printf("\n---------------------------------------\n\n");
}

void handleColour(TPacket *packet)
{
	printf("\n ------- ALEX COLOUR REPORT ------- \n\n");
	printf("Red:\t\t%d\n", packet->params[0]);
	printf("Green:\t\t%d\n", packet->params[1]);
	printf("Blue:\t\t%d\n", packet->params[2]);
	printf("Distance:\t\t%d\n", packet->params[3]))
	printf("\n---------------------------------------\n\n");	
}

void handleResponse(TPacket *packet)
{
	// The response code is stored in command
	switch(packet->command)
	{
		case RESP_OK:
			printf("Command OK\n");
		break;

		case COMMAND_GET_STATS:
			handleStatus(packet);
		break;

		case COMMAND_COLOUR:
			handleColour(packet);

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

void handleMessage(TMessage *packet)
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
    if (keyboardMode == 2) { return; }
	printf("Enter distance/angle in cm/degrees (e.g. 50) and power in %% (e.g. 75) separated by space.\n");
	printf("E.g. 50 75 means go at 50 cm at 75%% power for forward/backward, or 50 degrees left or right turn at 75%%  power\n");
	scanf("%d %d", &commandPacket->params[0], &commandPacket->params[1]);
}

void sendCommand(char command)
{
	TPacket commandPacket;

	if (keyboardMode == 1) {
        commandPacket.packetType = PACKET_TYPE_COMMAND_PARAM;
    } else {
        commandPacket.packetType = PACKET_TYPE_COMMAND_KEYBOARD;
    }

	switch(command)
	{
        // Changing the movement mode
        case '1':
            keyboardMode = 1;
            break;

        case '2':
            keyboardMode = 2;
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
		case 'c':
		case 'C':
			commandPacket.command = COMMAND_CLEAR_STATS;
			commandPacket.params[0] = 0;
			sendPacket(&commandPacket);
			break;

		case 'g':
		case 'G':
			commandPacket.command = COMMAND_GET_STATS;
			sendPacket(&commandPacket);
			break;

		case 'h':
		case 'H':
			commandPacket.command = COMMAND_COLOUR;
			sendPacket(&commandPacket);
			break;

        // Others
		case 'q':
		case 'Q':
			exitFlag=1;
			break;

		default:
			printf("Bad command\n");
	}
}
