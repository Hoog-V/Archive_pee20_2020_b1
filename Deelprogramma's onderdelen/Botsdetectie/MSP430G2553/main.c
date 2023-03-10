//******************************************************************************
//  MSP430G2553 Twee HC_SR04 sensor m.b.v. de capture optie van Timer A1.
//  Description: Deze code implementeert de capture optie van Timer A1 om een HC_SR04 ultrasoon
//  sensor aan te sturen. Hierbij wordt gebruik gemaakt van het 32 Khz. Aclk kristal om een accurate lezing te krijgen.
//
//  ACLK = LFXT1 = 32kHz crystal ; SMCLK = MCLK = 1 MHz;
//
//                 MSP430G2553
//             -----------------
//         /|\|              XIN|-
//          | |                 | 32kHz
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|--> HC_SR04 Sensor1 TRIG -->|
//            |             P1.3|--> HC_SR04 Sensor2 TRIG -->|
//            |       P2.0/TA1.0|<-- HC_SR04 Sensor2 ECHO <-|
//            |       P2.1/TA1.1|<-- HC_SR04 Sensor 1 ECHO <-|
//
//  W. Goh & Victor Hogeweij
//  Texas Instruments Inc en Hogeschool Rotterdam.
//  Maart 2021
//  Getest met CCS v10 and GCC compiler v9
//******************************************************************************
#include <msp430.h>
#include "UART.h"
#include "HC_SR04.h"


int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stoppen van watchdog timer
  DCOCTL = 0;                               // Laagste DCOx en MODx instellen
  BCSCTL1 = CALBC1_16MHZ;                    //DCO instellen op 1Mhz voor UART communicatie
  DCOCTL = CALDCO_16MHZ;                     //
  UART_init();                              //Opzetten van de UART communicatie
  HC_INIT();                                //Opzetten van de Botssensoren timers en pinnen
  while(1){
      //Print alleen de afstand tussen sensor1 en het object kleiner is dan 30.
      if(getDistanceS1() < 30){
      UART_printf("Distance: %u\n",getDistanceS1());
      }
      //Print alleen de afstand tussen sensor2 en het object kleiner is dan 30.
     // if(getDistanceS2() < 30){
     // UART_printf("DistanceS2: %u\n",getDistanceS2());
     // }
      //Even wachten...
      __delay_cycles(16);
}
}



