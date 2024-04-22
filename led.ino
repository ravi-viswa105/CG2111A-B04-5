/*
//Initialize Timer0
void InitTimer0(void) {
  //Set Initial Timer value
  TCNT0 = 0;
  //Place TOP timer value to Output compare register
  OCR0A = 50;
  //Set CTC mode
  // and make toggle PD6/OC0A pin on compare match
  TCCR0A = 0b01000010;
  // Enable interrupts.
  TIMSK0 |= 0b10;
}

void StartTimer0(void) {
  //Set prescaler 64 and start timer
  TCCR0B=0b00000101;
  // Enable global interrupts
  sei();
}
void ledOn() {
  PORTD |= 0b00000011;
  PORTD &= 0b11111101;
}
void ledOff() {
  PORTD &= 0b11111100;
  PORTD |= 0b00000010;
}

// Toggle the LED
void toggleLED() {
  static volatile int state = 1;

  if (state == 1) {
    ledOn();
    state = 0;
  } else {
    ledOff();
    state = 1;
  }
}

// Set up the ISR for TOV0A
ISR(TIMER0_COMPA_vect)
{
  ledcount++;
  if ((ledcount % 10000) == 0)
    toggleLED();
}
*/


void setupLED(){
  pinMode(red_pin , OUTPUT);
  pinMode(blue_pin , OUTPUT);
}
void LED_TOGGLE(){
  if(state){
    digitalWrite(red_pin , HIGH);
    digitalWrite(blue_pin , LOW);
    state = 0;
  }else{
    digitalWrite(red_pin , LOW);
    digitalWrite(blue_pin , HIGH);
    state = 1;
  }
}

