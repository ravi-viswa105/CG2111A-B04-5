// Include the library
#include "Servo.h"
#define distpersec 8.4
#define ServoPin 24
// Create the servo object
Servo cgservo;

void distance_forward(float dist){
  cgservo.write(90);
  cgservo.attach(24); // attach the servo to our servo object (DIGITAL PIN 45)
  cgservo.write(135); // rotate the motor forward
  delay(dist/distpersec*1000); // keep rotating

  cgservo.write(90); // stop the motor
  cgservo.detach();
}

void distance_reverse(float dist){
  cgservo.write(90);
  cgservo.attach(24); // attach the servo to our servo object (DIGITAL PIN 45)
  cgservo.write(45); // rotate the motor forward
  delay(dist/distpersec*1000); // keep rotating

  cgservo.write(90); // stop the motor
  cgservo.detach();
}
