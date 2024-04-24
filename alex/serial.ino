/*
 * Setup and start codes for serial communications
 * 
 */

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

int readSerial(char *buffer)
{

  int count=0;

  // Change Serial to Serial2/Serial3/Serial4 in later labs when using other UARTs

  while(Serial.available())
    buffer[count++] = Serial.read();

  return count;
}

// Write to the serial port. Replaced later with
// bare-metal code

void writeSerial(const char *buffer, int len)
{
  Serial.write(buffer, len);
  // Change Serial to Serial2/Serial3/Serial4 in later labs when using other UARTs
}