// RDF routine to test interval timer
// You need to set DS3231 for 1 Hz output using the RTClib.h, RTClib-master (1).zip library from Adafruit
// D4 input needs a 1 k ohm pullup resistor when used with a 3.3v DS3231
// Pins A4 and A5 need 4.7k ohm pullups for i2c connection
// Explained more in: https://arduino.stackexchange.com/questions/29873/how-to-set-up-one-second-interrupt-isr-for-ds3231-rtc
//22 March.Now working with bat detector input, using digital input.
//6 April now working with yellow LCD (OK for sunlight)Draws 90mA
//10 April Signal strength reading now seems to work
//17 April Working on DIP switch input for fiddle factor
/*-----( Import needed libraries )-----*/
#include <Wire.h>  // Comes with Arduino IDE
// Get the LCD I2C Library here:
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
// Move any other LCD libraries to another folder or delete them
// See Library "Docs" folder for possible commands etc.
#include <LiquidCrystal_I2C.h>
//#include <DS3231.h>
#include <Wire.h>
#include <toneAC.h>

int state, state0, previousState, new_state, time0, time1, time_delay, reading, diff, fiddle, strength, maximum = 0, pin = 0, i = 1, j, Flag1;

const byte interruptPin2 = 2;

byte val = 0;
//Test of i2c master writer
byte x = 0;

/*-----( Declare Constants )-----*/
/*-----( Declare objects )-----*/
// set the LCD address to 0x27 for a 16 chars 2 line display
// A FEW use address 0x3F
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

/*-----( Declare Variables )-----*/


void setup() {

  lcd.begin(16, 2);  // initialize the lcd for 16 chars 2 lines, turn on backlight

  // ------- Quick 3 blinks of backlight  -------------
  for (int i = 0; i < 3; i++)
  {
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
  }
  lcd.backlight(); // finish with backlight on

  //-------- Write characters on the display ------------------

  lcd.print("David's Pinger");

  lcd.setCursor(0, 1);

  lcd.print("Time, mS");

  delay(2000);
  lcd.clear();
  delay(200);

  Serial.begin(9600);
  pinMode(interruptPin2, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin2), pulse, RISING);
  pinMode(0, INPUT);//Get value for fiddle
  pinMode(1, INPUT);//Get value for signal strength
  pinMode(13, OUTPUT); // initialize digital pin 13 as an output.
  pinMode(4, INPUT);       // set pin to input
  digitalWrite(4, HIGH);   // turn on pullup resistors
  pinMode(5, INPUT);       // set pin to input for use as DIP switch
  digitalWrite(5, HIGH);   // turn on pullup resistors
  pinMode(6, INPUT);       // set pin to input for use as DIP switch
  digitalWrite(6, HIGH);   // turn on pullup resistors
  pinMode(7, INPUT);       // set pin to input for use as DIP switch
  digitalWrite(7, HIGH);   // turn on pullup resistors
  pinMode(8, INPUT);       // set pin to input for use as DIP switch
  digitalWrite(8, HIGH);   // turn on pullup resistors
  pinMode(9, INPUT);       // set pin to input for use as DIP switch
  digitalWrite(9, HIGH);   // turn on pullup resistors
}

void loop() {

  // int reading = analogRead(A0);//Read value for fiddle
  // reading = reading / 10; // Scale value down
  // fiddle = reading;

  // Read DIP switches
  if (digitalRead(5) == HIGH) {
    fiddle = 2;
  }
  else {
    fiddle = 0;
  }

  // Read DIP switches
  if (digitalRead(6) == HIGH) {
    fiddle = fiddle + 5;
  }
  else {
    fiddle = fiddle + 0;
  }

  // Read DIP switches
  if (digitalRead(7) == HIGH) {
    fiddle = fiddle + 7;
  }
  else {
    fiddle = fiddle + 0;
  }

  // Read DIP switches
  if (digitalRead(8) == HIGH) {
    fiddle = fiddle + 10;
  }
  else {
    fiddle = fiddle + 0;
  }

  // Read DIP switches
  if (digitalRead(9) == HIGH) {
    fiddle = fiddle - 10;
  }
  else {
    fiddle = fiddle + 0;
  }

  int strength = analogRead(A1);//Read value for signal strength

  //Find the max of values recorded
  if (strength > maximum) {
    maximum = strength;
  }

  previousState = state0; // save old state

  //  Loop while monitoring Digital pin 4
  if (digitalRead(4)) {
    digitalWrite(13, HIGH); // turn the LED on by making the voltage LOW
    state0 = HIGH;
  } else {
    digitalWrite(13, LOW); // turn the LED off (HIGH is the voltage level)
    state0 = LOW;
  }

  new_state = state0;

  //Next routine detects rising state
  if ((new_state != previousState) && (new_state > previousState))  {
    // Pin 4 went from LOW to HIGH.
    delay(0); // Set a delay
    time0 = micros(); // save time at rising state
    //Serial.print( time0);
    diff = (time0 - time1) / 1000; // Time difference in micro S

    // Test - set fiddle to make display zero;
    //fiddle = 0;

    diff = diff + fiddle;
    // diff = 0.343 * diff; //metres in air, speed 343 m/s
    //diff = 1.50 * diff; //metres in water, speed 1500 m/s

    Serial.print(diff);//Subtracted value to make value close to 0
    Serial.println(" ");

    Serial.print(maximum);//Subtracted value to make value close to 0
    Serial.println(" ");

    lcd.setCursor(0, 0);
    lcd.print(diff);
    delay(300);

    lcd.setCursor(0, 1);
    lcd.print(maximum);
    delay(300);

    lcd.clear();
    delay(200);

    maximum = 0; // reset maximum

    time0 = 0; //reset timer

  }

  // Serial.print(reading);
  // Serial.println(" ");

}
void pulse() {

  time1 = micros(); // save time at rising state
}
