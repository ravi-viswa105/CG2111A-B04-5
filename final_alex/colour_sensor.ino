#define S1 1<<5 // PL5, pin 44
#define S0 1<<1 // PL1, pin 48
#define S2 1<<1 // PB1, pin 52
#define S3 1<<3 // PL3, pin 46
#define sensorOut 1<<3 // PB3, pin 50

#define color_sensor_delay 50 //milliseconds
#define ShroudMouthToUltrasonic 7 //cm//to change
#define DistancetoStop 2 //cm//dist to stop from object

void setup_colour_sensor() {
  // outputs, set bit to 1
  DDRB |= S2 ;
  DDRL |= S0 | S1 | S3 ;
  // inputs, clear bits to 0
  DDRB &= ~(sensorOut) ;
  // set frequency scaling to 20% 
  PORTL |= S0 ; // set bit to 1
  PORTL &= ~(S1) ; // clear bit to 0
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
  // Setting RED (R) filtered photodiodes to be read
  PORTB &= ~(S2) ; // clear both pins
  PORTL &= ~(S3) ;
  delay(color_sensor_delay);

  // Reading the output frequency for RED
  red_freq = average_freq();
  delay(color_sensor_delay);

  // Setting GREEN (G) filtered photodiodes to be read
  PORTB |= S2 ; // set both pins
  PORTL |= S3 ;
  delay(color_sensor_delay);

  // Reading the output frequency for GREEN
  green_freq = average_freq();
  delay(color_sensor_delay);

  // Setting BLUE (B) filtered photodiodes to be read
  PORTB &= ~(S2) ; // clear S2
  PORTL |= S3 ; // set S3
  delay(color_sensor_delay);

  // Reading the output frequency for BLUE
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
