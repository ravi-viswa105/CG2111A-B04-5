#define trigPin 1<<0 // PG0, pin 41
#define echoPin 1<<2 // PG2, pin 39
#define simGround 1<<0 // PC0, pin 37
#define simVcc 1<<6 // PL6, pin 43

#define SPEED_OF_SOUND 340 // m/s
#define DIST_OFFSET 0 // cm

void setupUltrasonic(){

  DDRC &= ~(simGround); // input
  DDRG &= ~(echoPin) ; // input
  DDRG |= trigPin ; // output
  DDRL |= simVcc ; //output 
  
  PORTC &= ~(simGround) ; // write to LOW
  PORTL |= simVcc ; // write to HIGH
}

int getUltrasonicDistance() {
  PORTG &= trigPin ; // set trigPin to LOW
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  PORTG ^= trigPin ; // toggle it to HIGH
  delayMicroseconds(10); // delay
  PORTG ^= trigPin ; // back to LOW
  
  unsigned long duration = pulseIn(echoPin, HIGH , 5000);

  return (duration * SPEED_OF_SOUND) / 20000.0 - DIST_OFFSET; // Calculate the distance
}

void sendDistance() {
  int ultrasonicDist = getUltrasonicDistance();
  
  TPacket distancePacket;
  distancePacket.packetType = PACKET_TYPE_RESPONSE;
  distancePacket.command = RESP_DIST;
  distancePacket.params[0] = ultrasonicDist;
  sendResponse(&distancePacket);

  sendOK();
}