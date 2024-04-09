void handleError(TResult error);
void handleStatus(TPacket *packet);
void handleResponse(TPacket *packet);
void handleErrorResponse(TPacket *packet);
void handleMessage(TMessage *packet);
void handlePacket(TPacket *packet);
void sendPacket(TPacket *packet);
void *receiveThread(void *p);
void getParams(TPacket *commandPacket);
void sendCommand(char command);

void flushInput();
