//******************************************************************************
//  MSP430FR2476 Twee HC_SR04 sensoren m.b.v. de capture optie van Timer B0.
//  Description: Deze code implementeert de capture optie van Timer B0 om twee HC_SR04 ultrasoon
//  sensoren aan te sturen. Hierbij wordt gebruik gemaakt van het 32 Khz. Aclk kristal om een accurate lezing te krijgen.
//
//  ACLK = LFXT1 = 32kHz crystal ; SMCLK = MCLK = 16 MHz; Baudrate = 115200:
//
//                 MSP430FR2476
//             -----------------
//         /|\|              XIN|-
//          | |                 | 32kHz
//          --|RST          XOUT|-
//            |                 |
//            |             P1.4|--> TXD
//            |             P1.5|<-- RXD
//            |             P4.5|--> HC_SR04 Sensor2 TRIG -->|
//            |             P4.6|--> HC_SR04 Sensor1 TRIG -->|
//            |       P4.7/TB0.1|<-- HC_SR04 Sensor1 ECHO <-|
//            |       P5.0/TB0.2|<-- HC_SR04 Sensor2 ECHO <-|
//
//
//
//      Code deels gebaseerd op voorbeeld msp430g2xx3_ta_21.c. Te vinden in resource explorer
//      Aanpassingen die gemaakt zijn:
//      - Aangepast voor de msp430fr2476.
//      - Aangepast om timer b te gebruiken
//      - Aangepast om automatisch de lezingen te starten zonder in lpm te gaan
//      - Aangepast om op 16 mhz te werken -> andere formule
//      - Aangepast om twee sensoren te kunnen lezen
//  Victor Hogeweij
//  Hogeschool Rotterdam.
//  Maart 2021
//  Getest met CCS v10 and GCC compiler v9
//******************************************************************************
#include <msp430.h>
#include "UART.h"
#include "HC_SR04.h"

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stoppen van watchdog timer
  FRCTL0 = FRCTLPW | NWAITS_1;

  P2SEL0 |= BIT0 | BIT1;                  // P2.0~P2.1: crystal pins

  PM5CTL0 &= ~LOCKLPM5;                    // Disable the GPIO power-on default high-impedance mode
                                           // to activate 1previously configured port settings

  __bis_SR_register(SCG0);                 // disable FLL
  CSCTL3 |= SELREF__XT1CLK;               // Set REFO as FLL reference source
  CSCTL1 = DCORSEL_5;                       // DCORSEL=5, DCO Range = 16MHz
  CSCTL2 = FLLD_0 + 487;                  // DCODIV = 16MHz
  __delay_cycles(3);
  __bic_SR_register(SCG0);                // enable FLL
  CSCTL4 = SELMS__DCOCLKDIV | SELA__XT1CLK; // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                           // default DCODIV as MCLK and SMCLK source
  UART_init();                              //Opzetten van de UART communicatie
  HC_INIT();                                //Opzetten van de HC_SR04
  while(1){
      //Print alleen de afstand tussen sensor1 en het object kleiner is dan 30.
      if(getDistanceS1() < 15 && getDistanceS1() >0){
      UART_printf("Distance: %u\n",getDistanceS1());
      UART_printf("Omkeren");
      }
      //Print alleen de afstand tussen sensor2 en het object kleiner is dan 30.
      if(getDistanceS2() < 15 && getDistanceS2() > 0){
      UART_printf("DistanceS2: %u\n",getDistanceS2());
      UART_printf("Omkeren");
      }
      //als de afstanden groter zijn dan 15cm
      if(getDistanceS1() > 15 && getDistanceS2() > 15){
      UART_printf("Distance: %u\n",getDistanceS1());
      UART_printf("DistanceS2: %u\n",getDistanceS2());
      UART_printf("Doorrijden");
      }
      //Even wachten...
      __delay_cycles(1600000);
}
}


