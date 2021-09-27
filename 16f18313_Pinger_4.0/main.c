
/*
 Sept 2021. Pinger 4 - Ultrasonic pulse timed using a 16f18313 pic and an external 32.768 KHz clock crystal to give an accurate 1 second pulse. LED with fibre optic.
 
 * CR2032 capacity is 225mAh which gives 32 hours at 5mA
 * Farnell formula: mAh ÷ mA * 0.7 = estimated hours
 * 13/9/2020.Tested Pinger with waterproof Banggood ultrasonic transducer for receive and transmit. Range using 3v PIC is very low about 0.5m.
 * Moving on to using 5v 12F683 PIC to increase range and perhaps drop the need for a clock module
 * Using sensor from Banggood: Waterproof Ultrasonic Sensor Module Integrated Ranging Sensor Reversing Radar Measuring Distance. ID: 1369915
 * Sept 2020. Pulse interval 1 second. Duration 5.7mS. Frequency 42Khz. 4v Peak.
 * Sept 2020. Current draw from 2.7 to 0.2 mA on 5v.
 * Oct 2020. 330 micro Henry inductors in series with transducer. Range increased. Current draw 1.5 to 0.5 mA.
 * Oct 2020. Battery life with new batteries is from 11.10.2020 to 8.11.2020.So about 1 month, very good. Final cell voltages 1.58 & 1.6
 * Aug 2021. Trying with clock crystal using 18f47k42 looks promising - about 1.6 mA consumption. Accuracy looks good - less than 1 second in 12 hours
 * 1/9/2021  Now using 16f18313. Looks good. Scope shows 1 Hz pulse at 40MHz. Current draw 0.7mA normal, 2mA at pulse.
 * LED light pulse sent via fibre optics to outside the case
 * 1/9/2021  Using external crystal. 32.768 KHz clock crystal. Cylindrical 2x6.5mm; load capacitance: 12.5pF; equivalent resistance: 35 Kohms; frequency tolerance: +/-20PPM; 
 * 2/9/2021 9am Battery life tested started (+6v) 6/9/2021 voltage 5.4v Pulsing ended at about 4v on 9/9/2021 so only 3 days. Software needs to be improved.
 */



/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC16F18313
        Driver Version    :  2.00
 */

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
 */

#include "mcc_generated_files/mcc.h"

uint32_t unixtime = 1630590228;
int Countery = 0;

// Callback routine for Timer 1

void timer1_callback(void) {
    Countery = 1;
    unixtime = unixtime + 1;
    LATAbits.LATA2 = 1; // Flash LED
    __delay_ms(5);
    LATAbits.LATA2 = 0;
}

/*
                         Main application
 */
void main(void) {
    // initialize the device
    SYSTEM_Initialize();
    TMR0_SetInterruptHandler(timer1_callback);
    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();


    IDLEN = 1; // Idle mode enable

    while (1) {
        // Add your application code

        // Ultrasonic pulse   
        if ((Countery >= 1)&&(Countery < 250)) // Should be 50. 250  gives about 7mS pulse
        {

            //   GPIO = 0b000001; // Ultrasonic pulse
            LATAbits.LATA0 = 0; // Ultrasonic pulse
            LATAbits.LATA1 = 1; // Ultrasonic pulse
            __delay_us(12); // gives about 40 kHz
            //   GPIO = 0b000010; // Ultrasonic pulse
            LATAbits.LATA0 = 1; // Ultrasonic pulse
            LATAbits.LATA1 = 0; // Ultrasonic pulse
            __delay_us(5); //  gives about 40 kHz
            //  GPIObits.GP4 = 1; //Put LED on.
            //LATA = !LATA; // Toggle LED
            ++Countery; //Increment Counter
        } else {
            //   GPIO = 0b000000;
            LATAbits.LATA0 = 0; // Ultrasonic pulse
            LATAbits.LATA1 = 0; // Ultrasonic pulse  
            //LATA = !LATA; // Toggle LED
            SLEEP();

            //   printf("Unix time =  %ld \r\n", unixtime);
            //   printf("Counter =  %d \r\n", Counter);
            //   printf("Input pulse = %d Hall counter = %d Error signal= %d \r\n", ip_pulse, count, errsig);  
            //   __delay_ms(200);

        }
    }
}
/**
 End of File
 */
