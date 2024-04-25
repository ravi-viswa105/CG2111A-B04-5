
#define S1 44 //brown
#define S0 48 //orange
#define S2 52 //blue
#define S3 46 //green
#define sensorOut 50 //white

#define color_sensor_delay 50 //milliseconds
#define ShroudMouthToUltrasonic 7 //cm//to change
#define DistancetoStop 2 //cm//dist to stop from object



void setup_colour_sensor() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  
  // Setting frequency scaling to 20%
  digitalWrite(S0, HIGH);      
  digitalWrite(S1, LOW);
}

// Take 5 readings and return average frequency
int average_freq() {
  int reading;
  int total = 0;

  for (int i = 0; i < 5; i+=1) {
    reading = pulseIn(sensorOut, LOW);
    total += reading;
    delay(20);
  }
  
  return total / 5;
}

// Find the color of the paper
void findColor() { 
  // read RED by adjusting filter photodiodes
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  delay(color_sensor_delay);

  // find average frequency for RED
  red_freq = average_freq();
  delay(color_sensor_delay);

  // read GREEN by adjusting filter photodiodes
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  delay(color_sensor_delay);

  // find average frequency for GREEN
  green_freq = average_freq();
  delay(color_sensor_delay);

  // read BLUE by adjusting filter photodiodes
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  delay(color_sensor_delay);

  // find average frequency for BLUE
  blue_freq = average_freq();
  delay(color_sensor_delay);
}

void sendColor() {
  uint32_t currentdist = getUltrasonicDistance() - ShroudMouthToUltrasonic;
  if(currentdist > DistancetoStop){
    linearactuatordist = currentdist - DistancetoStop;
    if(linearactuatordist > 7){
      linearactuatordist = 7;
    }
    distance_forward(linearactuatordist);
  }

  findColor();
  TPacket colorPacket;
  colorPacket.packetType = PACKET_TYPE_RESPONSE;
  colorPacket.command = RESP_COLOR;
  
  colorPacket.params[0] = red_freq;
  colorPacket.params[1] = green_freq;
  colorPacket.params[2] = blue_freq;
  
  sendResponse(&colorPacket);  
  distance_reverse(linearactuatordist + 0.25);
  linearactuatordist = 0;
}
