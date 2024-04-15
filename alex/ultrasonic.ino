#define trigPin 41
#define echoPin 39
#define simGround 37
#define simVcc 43


#define SPEED_OF_SOUND 340 // m/s
#define DIST_OFFSET 0 // cm

void setupUltrasonic(){
  pinMode(trigPin, OUTPUT); // Sets the trigPin as Output
  pinMode(echoPin, INPUT); // Sets the echoPin as Input
  pinMode(simGround , INPUT);
  digitalWrite(simGround , LOW);
  pinMode(simVcc , OUTPUT);
  digitalWrite(simVcc , HIGH);
}

int getUltrasonicDistance() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
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
