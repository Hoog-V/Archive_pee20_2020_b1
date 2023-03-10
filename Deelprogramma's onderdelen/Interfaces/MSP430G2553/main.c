//******************************************************************************
//  MSP430G2553 Interfaces met RGB LED, Nokia 5110 display en drie knoppen.
//  Description: Deze code geeft de afstand en tijd weer op een nokia 5110 display en de knoppen
//  Zetten de tijd stil, starten of resetten de tijd. M.b.v. interrupts en pullups!!!
//
//           SMCLK = MCLK = 16 MHz;
//
//                 MSP430G2553
//             -----------------
//         /|\|             P1.3|--> Led Rood
//          | |             P1.4|--> Led Groen
//          --|RST          P1.5|--> Led Blauw
//            |                 |
//            |             P?.?|--> Display
//            |             P?.?|--> Display
//            |             P?.?|--> Display
//            |             P?.?|--> Display
//            |                 |
//            |             P2.3|<-- Start knop
//            |             P2.4|<-- Reset knop
//            |             P2.5|<-- Stop  knop
//
//  Robbert Berrevoets & Victor Hogeweij
//  PEE20 Hogeschool Rotterdam.
//  Maart 2021
//  Getest met CCS v10 en GCC compiler v9
//******************************************************************************


#include <msp430.h>
#include "interfaces.h"


int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  zet_klok_op_16mhz(); //Stelt de klokfrequentie in op 16mhz
  INIT_interfaces(); //Stelt de pinnen op de juiste manier in.
  while(1){
      if(StartKnop){
          zet_led_uit(led.R);
          zet_led_uit(led.G);
          zet_led_uit(led.B);
          zet_led_aan(led.R);
          StartKnop = 0;
      }
      else if(ResetKnop){
          zet_led_uit(led.R);
          zet_led_uit(led.G);
          zet_led_uit(led.B);
          zet_led_aan(led.G);
          ResetKnop = 0;
      }
      else if(StopKnop){
          zet_led_uit(led.R);
          zet_led_uit(led.G);
          zet_led_uit(led.B);
          zet_led_aan(led.B);
          StopKnop = 0;
      }
  }
}

