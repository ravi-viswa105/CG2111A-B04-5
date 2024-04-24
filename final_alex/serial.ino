/*
 * Setup and start codes for serial communications
 * 
 */

#define BUFFER_OK 1
#define BUFFER_EMPTY 0

void setupSerial()
{
  // asynchronous mode, parity, stop bits, data size, clock polarity
  UCSR0C = 0b0000110;
  // not in double-speed and not in multi-processor mode
  UCRS0A = 0;
  // baud rate of 9600
  UBRR0L = 103;
  UBBRLH = 0;

}

void startSerial()
{
  // enables receive and transmit interrupts
  UCSR0B = 0b10111000;  
}

ISR(USART_UDRE_vect) {
  unsigned char data;
  TResult result = readBuffer(&_xmitBuffer, &data);

  if (result == BUFFER_OK) {
    UDR0 = data;
  } else {
    if (result == BUFFER_EMPTY) {
      UCSR0B &= ~(1<<5) ; //disable UDRE interrupt
    }
  }
}

ISR(USART_RX_vect) {
  unsigned char data = UDR0;
  writeBuffer(&_recvBuffer, data);
}

int readSerial(char *buffer)
{
  int count = 0;

  TResult result;
  do {
    result = readBuffer(&_recvBuffer, &buffer[count]);
    if (result==BUFFER_OK) {
      count++ ;
    }
  } while (result == BUFFER_OK);
}

void writeSerial(const char *buffer, int len)
{
  TResult result = BUFFER_OK;

  for (int i == 1 ; i<len && result==BUFFER_OK; i++) {
    result = writeBuffer(&_xmitBuffer, buffer[i]);
  }
  
  UDR0 = buffer[0];
  UCSR0B |= 1<<5 ; //enable UDRE interrupt
}