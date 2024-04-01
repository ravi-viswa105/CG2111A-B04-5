#define S1 44
#define S0 48
#define S2 52
#define S3 46
#define sensorOut 50

#define color_sensor_delay 50 //milliseconds

unsigned long red_freq;
unsigned long green_freq;
unsigned long blue_freq;

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
  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  delay(color_sensor_delay);

  // Reading the output frequency for RED
  red_freq = average_freq();
  delay(color_sensor_delay);

  // Setting GREEN (G) filtered photodiodes to be read
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  delay(color_sensor_delay);

  // Reading the output frequency for GREEN
  green_freq = average_freq();
  delay(color_sensor_delay);

  // Setting BLUE (B) filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  delay(color_sensor_delay);

  // Reading the output frequency for BLUE
  blue_freq = average_freq();
  delay(color_sensor_delay);
}

void sendColor() {
  TPacket colorPacket;
  colorPacket.packetType = PACKET_TYPE_RESPONSE;
  colorPacket.command = RESP_COLOR;
  
  colorPacket.params[0] = red_freq;
  colorPacket.params[1] = green_freq;
  colorPacket.params[2] = blue_freq;
  
  sendResponse(&colorPacket);  
}