byte customChar0[8] = { 
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};

byte customChar1[8] = {
 0b00000,
 0b01010,
 0b01010,
 0b00000,
 0b10001,
 0b01110,
 0b00000,
 0b00000
};

void lcd_setup() {
  
  analogWrite(A8, 130); // overall LCD brightness
  analogWrite(A9, 130); // letter brightness
  analogWrite(A7, 0);
  
  lcd.begin(16, 2); //set up number of columns and rows of LCD
  lcd.createChar(0, customChar0); // create a new custom character (heart)
  lcd.createChar(1, customChar1); // create a new custom character (smiley)
}

void message_lcd(){ //To display "Help is on the way. Stay Calm" to victims on LCD screen
  lcd.setCursor(0, 0); //print message starts at cursor
  lcd.print("Help is on the");
  lcd.setCursor(0, 1);
  lcd.print("way. Stay Calm.");
  lcd.setCursor(15, 0);
  lcd.write((byte)0);  // print the custom char
  lcd.setCursor(15, 1);
  lcd.write((byte)1);
}

void ultrasonic_message_lcd() { //To display ultrasonic sensor readings on LCD for debugging
  lcd.setCursor(0, 0); //print message starts at cursor
  lcd.print("Ultrasonic");
  lcd.setCursor(0, 1);
  lcd.print(ultrasonicDist);
}



