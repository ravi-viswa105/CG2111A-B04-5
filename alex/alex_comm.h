//basic alex comm

/*SETUP
1)start serial communication on arduino then rpi
2)rpi try to communicate with arduino via startSerial() in serial.cpp
  2.1)if not successful , try for 5 times(can be changed)
  2.2)StartSerial sets up parity bits , stopbits , bytesize
3)rpi sets up receiver thread using pthread_create and assigns function to call when data is received from arduino
  3.1)thread allows multi-processing aka rpi can receive and send data concurrently
4)Create a packet with packet type PACKET_TYPE_HELLO and send via sendPacket() on rpi
  4.1)SendPacket() calls serialize() to break down packet into bytes and writes it to arduino via serialWrite
5)On arduino , in the main loop , packets are constantly read via the readPacket() function
  5.1)readPacket() calls readSerial() which reads the stream of bytes sent via RPI
    5.1.1)readSerial() reads byte by byte and puts into buffer[128],each byte in each index
    5.1.2)stops when there is no byte to read and returns the number of bytes aka length
  5.2)if len of btye received is 0 , packet_result = PACKET_INCOMPLETE
  5.3)else deserialize the buffer via deserialize()
    5.3.1)calls assemble() which copies over the bytes from the input buffer and transfer it into a output buffer.
    5.3.2)if not enough bytes received , adds it to a temp buffer to be reused next time it receives bytes
    5.3.3)if(packet is incomplete) , returns PACKET_INCOMPLETE and continue to wait till the required bytes are received
    5.3.4)if all is good, returns PACKET_OK
  5.4)if end_result is PACKET_OK , call handle_packet()
    5.4.1)checks packetType like command ,messsage,response etc
    5.4.2)if command type , call handle_command()
      5.4.2.1)checks the packet.command which says what the arduino should do
        5.4.2.1.1)do repective command like move forward,sense color
    5.4.3)else do nothing
  5.5)if result is PACKET_BAD , call sendBadPacket
    5.5.1)creates a packet of PACKET_TYPE_ERROR
    5.5.2)command = RESP_BAD_PACKET;
  5.6)if PACKET_BAD_CHECKSUM , do similarly
  5.7)else just do nothing.
6)Repeat
*/
/*NORMAL OPS
1)Alex-pi program takes in user commands and pass into sendCommand() parameters on RPI
  1.1)Takes in directional inputs
  1.2)sendCommand() creates a packet of type PACKET_TYPE_COMMAND
  1.3)Switch case inputs corresponding command into packet.command
    1.3.1)COMMAND_FORWARD , COMMAND_REVERSE ETC
2)On arduino , similar to SETUP
3)Back on RPI , when it receives data via receiver thread
  3.1)Calls receiveThread() which is setup in the pthread_create() function in setup
    3.1.1)ReceiveThread() calls serialRead() which returns len of bytes receieved
    3.1.1.1)if bytes not 0 then deserialize it
    3.1.1.2)if PACKET_OK , call handlePacket()
      3.1.1.2.1)checks packetType like command ,messsage,response etc and calls the respective function to handle
        a)reponse type calls handleResponse()
          a.1)calls handle_status , handlecolor depending on RESP_STATUS OR RESP COLOR
        b)Error type calls handle error and prints out error
        c)Message type prints out message
    3.1.2)else if result is bad,bad_checksum , call handle_error()
      3.1.2.1)print the error to screen accordingly
    3.1.3)if result is incomplete , do nothing
*/
/*
enum just specify each "defines" into a specific type
*/
/*
#define MS_PER_CM_AT_70PER 100
void forward(float distance)
{
  int current_dist = getUltrasonic();
  dir = (TDirection) FORWARD;
  move(speed, FORWARD);
  if(current_dist == 0){
    delay(int(distance/MS_PER_CM_AT_70PER));
    stop()
  }else{
    while(getUltrasonic() > current_dist - dist){
      delay(10);
    }
    stop();
  }
}
*/