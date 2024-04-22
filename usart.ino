/*
 * 
 * Alex Communication Routines.
 * 
 */
 
TResult readPacket(TPacket *packet)
{
    // Reads in data from the serial port and
    // deserializes it.Returns deserialized
    // data in "packet".
    
    char buffer[PACKET_SIZE];
    int len;

    len = readSerial(buffer);

    if(len == 0)
      return PACKET_INCOMPLETE;
    else
      return deserialize(buffer, len, packet);
}

void sendStatus()
{
  TPacket statusPacket;
  statusPacket.packetType = PACKET_TYPE_RESPONSE;
  statusPacket.command = RESP_STATUS;
  statusPacket.params[0] = leftForwardTicks;
  statusPacket.params[1] = rightForwardTicks;
  statusPacket.params[2] = leftReverseTicks;
  statusPacket.params[3] = rightReverseTicks;
  statusPacket.params[4] = leftForwardTurns;
  statusPacket.params[5] = rightForwardTurns;
  statusPacket.params[6] = leftReverseTurns;
  statusPacket.params[7] = rightReverseTurns;
  statusPacket.params[8] = forwardDist;
  statusPacket.params[9] = reverseDist;
  statusPacket.params[10] = deltaTicks;
  statusPacket.params[11] = targetTicks;
  sendResponse(&statusPacket);
}

void sendMessage(const char *message)
{
  // Sends text messages back to the Pi. Useful
  // for debugging.
 
  TPacket messagePacket;
  messagePacket.packetType=PACKET_TYPE_MESSAGE;
  strncpy(messagePacket.data, message, MAX_STR_LEN);
  sendResponse(&messagePacket);
}

void dbprintf(char *format, ...) {
  va_list args;
  char buffer[128];
  va_start(args, format);
  vsprintf(buffer, format, args);
  sendMessage(buffer);
} 

void sendBadPacket()
{
  // Tell the Pi that it sent us a packet with a bad
  // magic number.
  
  TPacket badPacket;
  badPacket.packetType = PACKET_TYPE_ERROR;
  badPacket.command = RESP_BAD_PACKET;
  sendResponse(&badPacket);
  
}

void sendBadChecksum()
{
  // Tell the Pi that it sent us a packet with a bad
  // checksum.
  
  TPacket badChecksum;
  badChecksum.packetType = PACKET_TYPE_ERROR;
  badChecksum.command = RESP_BAD_CHECKSUM;
  sendResponse(&badChecksum);  
}

void sendBadCommand()
{
  // Tell the Pi that we don't understand its
  // command sent to us.
  
  TPacket badCommand;
  badCommand.packetType=PACKET_TYPE_ERROR;
  badCommand.command=RESP_BAD_COMMAND;
  sendResponse(&badCommand);
}

void sendBadResponse()
{
  TPacket badResponse;
  badResponse.packetType = PACKET_TYPE_ERROR;
  badResponse.command = RESP_BAD_RESPONSE;
  sendResponse(&badResponse);
}

void sendOK()
{
  TPacket okPacket;
  okPacket.packetType = PACKET_TYPE_RESPONSE;
  okPacket.command = RESP_OK;
  sendResponse(&okPacket);  
}

void sendResponse(TPacket *packet)
{
  // Takes a packet, serializes it then sends it out
  // over the serial port.
  char buffer[PACKET_SIZE];
  int len;

  len = serialize(buffer, packet, sizeof(TPacket));
  writeSerial(buffer, len);
}

void handleCommandParam(TPacket *command)
{
  switch(command->command)
  {
    // For movement commands, param[0] = distance, param[1] = speed.
    case COMMAND_FORWARD:
        sendOK();
        forward((double) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_REVERSE:
        sendOK();
        backward((double) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_TURN_LEFT:
        sendOK();
        left((double) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_TURN_RIGHT:
        sendOK();
        right((double) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_STOP:
        sendOK();
        stop();
      break;
    case COMMAND_GET_STATS:
        sendOK();
        sendStatus();
      break;
    case COMMAND_CLEAR_STATS:
        sendOK();
        clearCounters();
      break;
    case COMMAND_COLOR:
      findColor();
      sendColor();
      sendOK();
      break;
    case COMMAND_DIST:
      sendDistance(); // OK packet in sendDistance
      break;

    case COMMAND_LCD:
      if(toggleLCD){
        analogWrite(A8, 0); // overall LCD brightness
        analogWrite(A9, 0); // letter brightness
        toggleLCD = false;
      }else{
        lcd_setup();
        message_lcd();
        toggleLCD = true;
      }
      break;
        
    default:
      sendBadCommand();
  }
}

void handleCommandKeyboard(TPacket *command)
{
  switch(command->command)
  {
    // For movement commands, param[0] = distance, param[1] = speed.
    case COMMAND_FORWARD:
        sendOK();
        forward(-1, command->params[0]);
      break;
    case COMMAND_REVERSE:
        sendOK();
        backward(-1, command->params[0]);
      break;
    case COMMAND_TURN_LEFT:
        sendOK();
        left(-1, command->params[0]);
      break;
    case COMMAND_TURN_RIGHT:
        sendOK();
        right(-1, command->params[0]);
      break;
    case COMMAND_STOP:
        sendOK();
        stop();
      break;
    case COMMAND_GET_STATS:
        sendOK();
        sendStatus();
      break;
    case COMMAND_CLEAR_STATS:
        sendOK();
        clearCounters();
      break;
    case COMMAND_COLOR:
      findColor();
      sendColor();
      sendOK();
      break;
    case COMMAND_DIST:
      sendDistance(); // OK packet in sendDistance
      break;

    case COMMAND_LCD:
      if(toggleLCD){
        analogWrite(A8, 0); // overall LCD brightness
        analogWrite(A9, 0); // letter brightness
        toggleLCD = true;
      }else{
        lcd_setup();
        message_lcd();
        toggleLCD = false;
      }
      break;
        
    default:
      sendBadCommand();
  }
}

void handleCommandTime(TPacket *command)//note collision not activated here
{
  switch(command->command)
  {
    // For movement commands, param[0] = distance, param[1] = speed.
    case COMMAND_FORWARD:
        sendOK();
        forward(-1, 100);
        delay(command->params[0]);
        stop();
      break;
    case COMMAND_REVERSE:
        sendOK();
        backward(-1, 100);
        delay(command->params[0]);
        stop();
      break;
    case COMMAND_TURN_LEFT:
        sendOK();
        left(-1, 100);
        delay(command->params[0]);
        stop();
      break;
    case COMMAND_TURN_RIGHT:
        sendOK();
        right(-1, 100);
        delay(command->params[0]);
        stop();
      break;
    case COMMAND_STOP:
        sendOK();
        stop();
      break;
    case COMMAND_GET_STATS:
        sendOK();
        sendStatus();
      break;
    case COMMAND_CLEAR_STATS:
        sendOK();
        clearCounters();
      break;
    case COMMAND_COLOR:
      findColor();
      sendColor();
      sendOK();
      break;
    case COMMAND_DIST:
      sendDistance(); // OK packet in sendDistance
      break;

    case COMMAND_LCD:
      if(toggleLCD){
        analogWrite(A8, 0); // overall LCD brightness
        analogWrite(A9, 0); // letter brightness
      }else{
        lcd_setup();
        message_lcd();
      }
      break;

    default:
      sendBadCommand();
  }
}

void handlePacket(TPacket *packet)
{
  switch(packet->packetType)
  {
    case PACKET_TYPE_COMMAND_PARAM:
      handleCommandParam(packet);
      break;

    case PACKET_TYPE_COMMAND_KEYBOARD:
      handleCommandKeyboard(packet);
      break;
    case PACKET_TYPE_COMMAND_TIME :
      handleCommandTime(packet);
      break;
    
    case PACKET_TYPE_RESPONSE:
      break;

    case PACKET_TYPE_ERROR:
      break;

    case PACKET_TYPE_MESSAGE:
      break;

    case PACKET_TYPE_HELLO:
      break;
  }
}

void waitForHello()
{
  int exit=0;

  while(!exit)
  {
    TPacket hello;
    TResult result;
    
    do
    {
      result = readPacket(&hello);
    } while (result == PACKET_INCOMPLETE);

    if(result == PACKET_OK)
    {
      if(hello.packetType == PACKET_TYPE_HELLO)
      {
        sendOK();
        exit=1;
      }
      else
        sendBadResponse();
    }
    else
      if(result == PACKET_BAD)
      {
        sendBadPacket();
      }
      else
        if(result == PACKET_CHECKSUM_BAD)
          sendBadChecksum();
  } // !exit
}
