// RDF routine for underwater distance measurement using time difference between 2 clock modules.
// Feb 2020. Tested in pond - work well measuring distance over 10 m or so.
// Hardware for ultrasonic signal source is transducer+preamplifier+filter+amplifier+Schmitt trigger comparator
// You need to set DS3231 for 1 Hz output using the RTClib.h, RTClib-master (1).zip library from Adafruit
// D2 is the input pin for 1 Hz GPS time signal or DS3231(goes to interrupt)
// D3 is the interrupt input for audio input
// NOT USED. D4, the audio or test timer, input needs a 1 k ohm pullup resistor when used with a 3.3v DS3231
// Pin A1 is analogue input for signal strength.
// Pins A4 and A5 need 4.7k ohm pullups for i2c connection
// LED on Pin 13 for pulse routine from interrupt for GPS input time on Pin 2
// LED on Pin 10 indicates audio input on Pin 2.
// Audio output for piezo on Pin 11 indicates audio input on Pin 2.
// Explained more in: https://arduino.stackexchange.com/questions/29873/how-to-set-up-one-second-interrupt-isr-for-ds3231-rtc
// 6 April 2019  Now working with yellow LCD (Better display for sunlight)Draws 90mA
// 17 April 2019 Working on DIP switch input for fiddle factor used to get the time difference closer to zero
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

int state, state0, previousState, new_state, reading, fiddle, strength, maximum = 0, pin = 0, i = 1, j, Flag1;
unsigned long time0, time1, time2, time3, diff, start_time;


const byte interruptPin2 = 2;
const byte interruptPin3 = 3;

byte val = 0;
//Test of i2c master writer
byte x = 0;

/*-----( Declare Constants )-----*/
/*-----( Declare objects )-----*/
// set the LCD address to 0x27 or 0x3F for a 16 chars 2 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

// lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

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

  lcd.print("Time diff,mS");

  delay(2000);
  lcd.clear();
  delay(200);

  Serial.begin(9600);
  pinMode(interruptPin2, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin2), timer_pulse, RISING);

  pinMode(interruptPin3, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin3), signal_pulse, RISING);

  pinMode(0, INPUT);//Get value for fiddle
  pinMode(1, INPUT);//Get value for signal strength
  pinMode(13, OUTPUT); // initialize digital pin 13 as an output.
  pinMode(10, OUTPUT); // initialize digital pin 10 as an output.
  pinMode(11, OUTPUT); // initialize digital pin 11 as an output.
  pinMode(4, INPUT);       // set pin to input
  //digitalWrite(4, HIGH);   // turn on pullup resistors
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

  //Set initial state for sounder and LEDs
  digitalWrite(13, 0);
  digitalWrite(10, 0);
  noTone (11);

  int reading = analogRead(A0);//Read value for fiddle
  reading = reading / 10; // Scale value down
  fiddle = reading;

  // Read DIP switches
  if (digitalRead(9) == HIGH) {
    fiddle = 200;
  }
  else {
    fiddle = 0;
  }

  // Read DIP switches
  if (digitalRead(8) == HIGH) {
    fiddle = fiddle + 100;
  }
  else {
    fiddle = fiddle + 0;
  }

  // Read DIP switches
  if (digitalRead(7) == HIGH) {
    fiddle = fiddle + 100;
  }
  else {
    fiddle = fiddle + 0;
  }

  // Read DIP switches
  if (digitalRead(6) == HIGH) {
    fiddle = fiddle + 100;
  }
  else {
    fiddle = fiddle + 0;
  }

  // Read DIP switches
  if (digitalRead(5) == HIGH) {
    fiddle = fiddle + 50;
  }
  else {
    fiddle = fiddle + 0;
  }

  strength = analogRead(1);//Read value for signal strength

  //Find the max of values recorded
  if (strength > maximum) {
    maximum = strength;
  }
  diff = ((time3 - time2) / 1000); // Time difference in mS
}

//Bring in fiddle factor
//diff = diff - fiddle;

// Put boundaries in for difference values
//if ((diff > 1000) && (diff < 0)) {
// diff = 1;

// diff = 0.343 * diff; //metres in air, speed 343 m/s
//diff = 1.50 * diff; //metres in water, speed 1500 m/s

//Serial.print(diff);//Subtracted value to make value close to 0
//Serial.println(" ");

//Serial.print(maximum);//Subtracted value to make value close to 0
//Serial.println(" ");

lcd.setCursor(0, 0);
lcd.print(diff);
delay(300);

lcd.setCursor(0, 1);
lcd.print(maximum);
delay(350);

lcd.clear();
delay(200);

Serial.print(diff);
Serial.println(" ");
Serial.print(fiddle);
Serial.println(" ");

}
void timer_pulse() {
  time2 = micros(); // save time at rising state
  digitalWrite(13, 1); // toggle the state.Turn the LED on by making the voltage HIGH

}

void signal_pulse() {
  time3 = micros(); // save time at rising state
  digitalWrite(10, 1); // turn the LED on by making the voltage HIGH
  tone(11, 1500);

}
