/*
  RDF routine for underwater distance measurement using time difference between 2 clock modules.
  April 2020. Putting in potentiometer to give null for time difference
  Feb 2020. Tested in pond - works measuring distance over 10 m or so.
  Hardware for ultrasonic signal source is transducer+preamplifier+filter+amplifier+Schmitt trigger comparator
  You need to set DS3231 for 1 Hz output using the RTClib.h, RTClib-master (1).zip library from Adafruit
  D2 is the input pin for 1 Hz DS3231 or GPS time signal(goes to interrupt). Needs 1k ohm pullup resistor to 5v with a 3.3v DS3231
  D3 is the interrupt input for audio input. Needs 1k ohm pullup resistor to 5v when used with a 3.3v DS3231
  Pin A1 is analogue input for signal strength.
  Pin A2 is analogue input for potentiometer input (100k ohm)for null
  Pins A4 and A5 need 4.7k ohm pullups for i2c connection. NOT USED
  LED on Pins 12 & 13 for pulse routine from interrupt for GPS input time on Pin 2
  LED on Pin 10 indicates audio input on Pin 2.
  Audio output for piezo on Pin 11 indicates audio input on Pin 2.
  Explained more in: https://arduino.stackexchange.com/questions/29873/how-to-set-up-one-second-interrupt-isr-for-ds3231-rtc
  6 April 2019  Now working with yellow LCD (OK for sunlight)Draws 90mA
  17 April 2019 Working on DIP switch input for fiddle factor used to get the time difference closer to zero (Not used-over complicated. Feb 2020)
  Working on using OLED dipslay (LCD gave intermittent problems)
  30.5.2020 Experienced some overheating problems of Arduino so trying voltage level conversion. (3.3v VDD, 470 ohm resistors in series, 1k ohm resistor then connected to earth for SDA and SCL to convert to 3.3 input.) 4.7 k ohm pullup resistors also fitted for i2c requirement.
  29.10.2020. NOTE: oLED needs level conversion from 5v to 3.3v.IIC I2C Bi-Directional Logic Level Shifter Converter
  Module 5V to 3.3V from eBay works well(puretek-innovations 272249807035 )
  Details of OLED display from Banggood:
  White 0.96 Inch OLED I2C IIC Communication Display 128*64 LCD Module
  VCC: 3.3-5V
  GND: Ground
  SCL: Serial Clock
  SDA: Serial Data
  Size: 0.96 inch
  Resolution: 128*64
  Color: white
  Viewing angle: >160°
  Support platform: , 51 series, MSP430 series, STM32/2, chip, etc.
  Ultra low power consumption: 0.04W during normal operation
  Wide voltage support: DC 3.3V-5V
  Working temperature: -30 ° C ~ 80 ° C
  Driver chip: SSD1306
  Communication method: IIC, only 2 I/O ports
  Backlight: OLED self-illumination, no backlight required
  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)
  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.
*/

#include <Arduino.h>
#include <U8g2lib.h>
#include <toneAC.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

String inString;   //  Make global
long bitt = 0;   //  Make global

int state, state0, previousState, new_state, reading, fiddle, nuller, strength, maximum = 0, pin = 0, i = 1, j, Flag1;
unsigned long time0, time1, time2, time3, diff, start_time;

const byte interruptPin2 = 2;
const byte interruptPin3 = 3;

/*
  U8g2lib Example Overview:
  Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
  Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
  U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
  This is a page buffer example for oLED.
  Some fonts:
  // u8g2.setFont(u8g2_font_ncenB14_tr);
  // 11 Pixel height font
  //u8g2.setFont(u8g2_font_ncenB10_tr);
  // 14 Pixel font
  //u8g2.setFont(u8g2_font_profont22_mf);
  // 16 Pixel font
  //u8g2.setFont(u8g2_font_crox3hb_tn);

  // 24 Pixel font
  //u8g2.setFont(u8g2_font_helvB24_tr_short.png);

  // 49 Pixel Height
  //u8g2.setFont(u8g2_font_fur49_tn);
  //u8g2.drawStr(20, 55, b);
  // 62 Pixel Height
  u8g2.setFont(u8g2_font_logisoso62_tn);
*/

// Please UNCOMMENT one of the contructor lines below
// U8g2 Contructor List (Picture Loop Page Buffer)
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected
//U8G2_NULL u8g2(U8G2_R0);  // null device, a 8x8 pixel display which does nothing
//U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 12, /* dc=*/ 4, /* reset=*/ 6); // Arduboy (Production, Kickstarter Edition)
//U8G2_SSD1306_128X64_NONAME_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_1_3W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_ALT0_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   // same as the NONAME variant, but may solve the "every 2nd line skipped" problem
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* reset=*/ 8);
U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

// End of constructor list. RDF removed a lot of these.

void setup(void) {

  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Wake-up USB!");
  delay(1000); // We wait 1 second

  //Test oled
  u8g2.begin();
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 15, "Pinger");
  } while ( u8g2.nextPage() );
  delay(1000);

  attachInterrupt(digitalPinToInterrupt(interruptPin2), timer_pulse, RISING); 

  pinMode(interruptPin3, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin3), signal_pulse, RISING); 

  pinMode(1, INPUT);//Get value for signal strength
  pinMode(13, OUTPUT); // initialize digital pin 13 as an output.
  pinMode(12, OUTPUT); // initialize digital pin 12 as an output.
  pinMode(33, OUTPUT); // initialize digital pin 11 as an output.
  pinMode(10, OUTPUT); // initialize digital pin 10 as an output.
  pinMode(4, INPUT);       // set pin to input
  // digitalWrite(2, HIGH);   // turn on pullup resistors
  //digitalWrite(3, HIGH);   // turn on pullup resistors


}
void loop(void) {

  //Set initial state for sounder and LEDs
  digitalWrite(13, 0);
  digitalWrite(12, 0);
  digitalWrite(10, 0);
  noTone (11);

  // The null analog reading (which goes from 0 - 1023)
  nuller = analogRead(2);//Read value for null indicator
  nuller = nuller / 2; // fiddle factor

  strength = analogRead(1);//Read value for signal strength

  //Find the max of values recorded. NOW CHANGED TO LOOK AT MINIMUM
  //if (strength > maximum) {
  if (strength < maximum) {
    maximum = strength;
  }

  long diff = ((time2 - time3) / 1000); // Time difference in mS

  //Test - take nuller out
  //nuller = 0;

  diff = diff - nuller; // Put null adjustment in

  if (diff > 1000) { // Capture silly values
    diff = -1;
  }

  // Print section
  // Convert integer to character for display ( https://www.instructables.com/id/Converting-integer-to-character/)

  // Test
  //bitt = 999;

  // Test nul
  //bitt = nuller;

  bitt = diff;

  long a = bitt;

  char b[5]; // define character variable

  String str; // define string variable

  str = String(a); // convert to string

  str.toCharArray(b, 5);

  // Print to oLED
  u8g2.firstPage();
  do {
    // 62 Pixel Height. Quite large
    u8g2.setFont(u8g2_font_logisoso62_tn);

    // 22 Pixel height
    //u8g2.setFont(u8g2_font_profont22_mf);

    u8g2.drawStr(10, 65, b);
  } while ( u8g2.nextPage() );

  delay(1000);

  Serial.print(diff);
  //Serial.print(nuller);
  Serial.println(" ");

  //Serial.print(maximum);
  //Serial.println(" ");

}

void timer_pulse() {
  time2 = micros(); // save time at rising state
  digitalWrite(12, 1); // toggle the state.Turn the LED on by making the voltage HIGH

}

void signal_pulse() {
  time3 = micros(); // save time at rising state
  digitalWrite(10, 1); // turn the LED on by making the voltage HIGH
  tone(11, 1500);

}
