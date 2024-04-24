//volatile int count;
volatile int ledcount;

void InitTimer0(void) {
  TCNT0 = 0; // initial timer value
  OCR0A = 255; // TOP timer value in compare register
  // TCCR0A already defined for delay() function in ArduinoCore-avr
  // TCCR0A = 0b01000010; // toggle OC0A on compare match, and set WGM to CTC
  TIMSK0 |= 0b10; // enable output compare match for OCR0A
}

void StartTimer0(void) {
  // TCCR0B already defined for delay() function in ArduinoCore-avr
  // TCCR0B=0b00000101; // set WGM to CTC with TCCR0B, and prescaler 1024
  sei();
}

void redFlash() {
  PORTD |= 0b00000011;
  PORTD &= 0b11111101;
}
void blueFlash() {
  PORTD &= 0b11111100;
  PORTD |= 0b00000010;
}

// Toggle the LED
void toggleLED() {
  static volatile int state = 1;

  if (state == 1) {
    redFlash();
    state = 0;
  } else {
    blueFlash();
    state = 1;
  }
}

// Set up the ISR for TOV0A
ISR(TIMER0_COMPA_vect)
{
  ledcount++;
  if (ledcount == 125000) { 
    toggleLED();
    ledcount = 0;
  }
}