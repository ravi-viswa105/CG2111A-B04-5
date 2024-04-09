// Include the library
#include "Servo.h"

// Create the servo object
Servo cgservo;

// Setup section to run once
void setup() {
  cgservo.attach(45); // attach the servo to our servo object (DIGITAL PIN 45)
  cgservo.write(90); // stop the motor, any value greater than 90 causes the servo to rotate clockwise, and determines the speed
}

// Loop to keep the motor turning
void loop() {
  cgservo.write(135); // rotate the motor forward
  delay(500); // keep rotating

  cgservo.write(90); // stop the motor
  cgservo.detach();
  delay(3000); // stay stopped

  cgservo.attach(45);
  cgservo.write(45); // rotate the motor backwards
  delay(500); // keep rotating for 5 seconds (5000 milliseconds)
}