// December 2019
// 8th October 2019. Delay increased to 20 mS will reduce battery life.
// March 2019-Battery life - only about 3 hours - needs more work
// Feb 22nd. Now using toneAC for higher sound output
// Uses 1 Hz output from DS3231 to an interrupt
// 1 Hz output from DS3231 goes to Interrupt pin D2
// +3.3v pin to + on DS3231
// Transmit ultrasonic output on Digital pins 9,10 using push/pull on toneAC for increased output
// CR2032 + goes to Vin pin
//Power saving comments:
// Without sleep about 16mA max.
//With sleep about 10mA max.
//ADC switched off - no visible change
//Power LED on board scratched out - about 7mA max.
//Low power library not used yet.
//Using https://github.com/n0m1/Sleep_n0m1
#include <Sleep_n0m1.h>
#include <toneAC.h> // Higher volume ultrasound using push-pull
//#include <LowPower.h>
Sleep sleep;

#define intPin 2

void setup()
{

}

void loop()
{
//Power save
// disable ADC
ADCSRA = 0;  

ultrapulse();
sleep.pwrDownMode(); //set sleep mode

//Sleep till interrupt pin equals a particular state.

sleep.sleepPinInterrupt(intPin,RISING); //(interrupt Pin Number, interrupt State)
}

//Ultrasonic pulse function
//Transmit ultrasonic output on Pin 9,10 using push/pull on toneAC
void ultrapulse() {
  toneAC(40000);//Normally 40000
  delay(1); // Was 2
  noToneAC();
}
