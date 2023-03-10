//***************************************************************************************
//  MSP430 robot interfaces voorbeeld
//
//  MSP430FR2476 Family User's Guide      : https://www.ti.com/lit/ug/slau445i/slau445i.pdf
//  MSP430FR2476 Data Sheet              : https://www.ti.com/lit/ds/symlink/msp430fr2476.pdf
//  PCD8544 Data Sheet (Nokia 5110 MCU) : https://www.sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf
//
//  My setup:
//
//         NOKIA 5110 LCD                                               MSP430FR2476
//      -------------------                                         -----------------
//      |               BL|<-- Back-light --------------------------|   P4.2   P2.5 | ----> LED Rood
//      |              CLK|<-- Clock -------------------------------|   P1.1   P2.6 | ----> LED Groen
//      |              DIN|<-- Data Input --------------------------|   P1.2   P2.7 | ----> LED Blauw
//      |               DC|<-- Data/Command (high/low) -------------|   P4.4        |
//      |              RST|<-- Reset -------------------------------|   P4.1   P3.5 | <---- Start knop
//      |            (S)CE|<-- Chip Enable (active low) ------------|   P4.3   P3.6 | <---- Stop knop
//      |              GND|<-- Ground ------------------------------|   GND    P3.7 | <---- Reset knop
//      |              VCC|<-- Vcc +3..5V --------------------------|   VCC         |        --------------------
//      -------------------                                         |          P2.0 | <---- |32768hz oscillator |
//                                                                  |          P2.1 | <-----|                   |
//                                                                  -----------------        --------------------
//
//    Deze code bevat een paar functies uit de code van RobG : http://forum.43oh.com/topic/1312-nokia-5110-display
//
//

#include <msp430.h>
#include <stdio.h>
#include "interfaces.h"


/**
 * main.c
 */


//
int main(void){
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer
    zet_klok_op_16mhz(); //Stelt de smclk in op 16 Mhz
    INIT_interfaces(); //initialiseert de interfaces
    Display_Initialize();
    while(1){
        /*
         * Testfunctie. Test of de led werkt en of de
         * communicatie tussen de main loop en de interrupt functie van de knoppen
         * goed werkt.
         *
         * Als de startknop(P3.5) is ingedrukt gaat de rode led branden
         * Als de stopknop(P3.6) is ingedrukt gaat de groene led branden
         * Als de resetknop(P3.7) is ingedrukt gaat de blauwe led branden
         *
         * Geschreven door: Victor
         */
        switch(knopstatus){
        case 'D':
            zet_led_uit(led.R); //Zet alle leds uit
            zet_led_uit(led.G);
            zet_led_uit(led.B);
            zet_led_aan(led.R); //Zet de rode led aan
            knopstatus = 0; //reset de knopstatus zodat hij niet blijft draaien
            break;
        case 'S':
            zet_led_uit(led.R);
            zet_led_uit(led.G);
            zet_led_uit(led.B);
            zet_led_aan(led.G);
            knopstatus = 0;
            break;
        case 'R':
            zet_led_uit(led.R);
            zet_led_uit(led.G);
            zet_led_uit(led.B);
            zet_led_aan(led.B);
            knopstatus = 0;
            break;
        }
    }
}
