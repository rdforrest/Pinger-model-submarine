// Based on ADC sound input. Including transmit pulse for transponder.
// Feb 5th. Transponder workingbut needs testing.
#include <Wire.h>  // Comes with Arduino IDE

const byte LED = 13;

int state, state0, new_state, time0, time1, reading, pin = 0, i = 1;

//https://arduino.stackexchange.com/questions/39942/measuring-time-between-two-photoresistors-and-printing-time-on-lcd
const int THRESHOLD_LOW  = 470;
const int THRESHOLD_HIGH = 490;
int previousState;

void setup ()
{

  Serial.begin(9600);
  Wire.begin();
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(0, INPUT);

}  // end of setup

void loop ()
{
  int reading = analogRead(A0);
  //Test
  // reading = 200;

  //Hysteresis section
  if (previousState == LOW && reading >= THRESHOLD_HIGH) {
    previousState = HIGH;
  } else if (previousState == HIGH && reading < THRESHOLD_LOW) {
    previousState = LOW;
  } else {
    previousState = previousState;
  }

  state0 = previousState;
  new_state = state0;

  //Next routine detects falling state
  if (state0 == HIGH && new_state == LOW)  {
    // ADC 0 went from HIGH to LOW.
  }
  state0 = new_state;  // state has changed

  // Serial.println(previousState);
  Serial.println(reading);
  Serial.println(state0);
  //delay (50);

  if (state0 == LOW) {

    digitalWrite(13, HIGH);   // turn the LED on
    time0 = micros();

    // TEST transmit once Transponder ultrasonic output on Pin 4,5
    tone(4, 40000);
    tone(5, 40000);
    delay(20);
    noTone(4);
    noTone(5);

    state0 = HIGH;

    delay(1000);              // wait for a little, was 50

  }

  else
    digitalWrite(13, LOW);


  time1 = micros();
  Serial.print(time1 - time0);
  Serial.println(" time delay micro s");

  //TEST TURNED OFF Regular ultrasonic output on Pin 4,5
  //tone(4, 40000);
  //tone(5, 40000);
  //delay(200);
  //noTone(4);
  //noTone(5);
  //delay(3000);

  state = state0;  // save for next time

}




