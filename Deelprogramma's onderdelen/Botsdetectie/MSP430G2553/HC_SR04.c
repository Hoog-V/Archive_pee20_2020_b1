/*
 * HC_SR04.c
 *
 *  Created on: Mar 10, 2021
 *      Author: victor
 */
#include <msp430.h>
#include "HC_SR04.h"

int toggleSensor = 0; //Deze variabele wordt gebruikt in de interrupt om te schakelen van sensor.
unsigned char Count, First_Time, CountS2, First_TimeS2; //Deze variabelen zijn nodig in de capture interrupts van sensor 1 en 2
unsigned int REdge1, FEdge,REdge1S2, FEdgeS2, distance, distanceS2; //Deze variabelen zijn nodig in de capture interrupt van sensor 2
void HC_INIT(){
    //Instellen van de pinnen
    P1DIR |= BIT0 + BIT3;                            // P1.0 en P1.3 initialiseren voor de trigger signalen van de hc_sr04 sensoren.
    P1OUT &= ~BIT0 + ~BIT3;                          // P1.0 en P1.3 uitzetten.
    P2DIR &= ~BIT1 + ~BIT0;                          // P2.0/TA1.0 en P2.1/TA1.1 instellen als ingangen
    P2SEL |= BIT1 + BIT0;                            // P2.0/TA1.0 en P2.1/TA1.1 instellen als timer ingangen
    // Instellen van timers
    CCTL0 = CCIE;                              // Instellen van timer0 interrupt voor het triggeren van de sensoren
    CCR0 = 500-1;                              // Instellen van de periode voor de timer0 interrupt
    TA0CTL = TASSEL_1 + MC_1;                  // Timer instellen op ACLK en Upmode
   // TA1CCTL0 = CAP + CM_3 + CCIE + SCS + CCIS_0; //Timer 1 capture register 0 instellen om timer capture te doen voor sensor 2.
    TA1CCTL1 = CAP + CM_3 + CCIE + SCS + CCIS_0; //Timer 1 capture register 0 instellen om timer capture te doen voor sensor 1.
    TA1CTL |= TASSEL_2 + MC_2 + TACLR + ID_3;        // SMCLK, Cont Mode; start timer
    // Variabelen de juiste start waarde geven
    Count = 0x0;
    First_Time = 0x01;
    CountS2 = 0x0;
    First_TimeS2 = 0x01;
    __bis_SR_register(GIE);   //Interrupts aanzetten
}

unsigned int getDistanceS1(void){
return distance;
}
unsigned int getDistanceS2(void){
return distanceS2;
}
void stopHCSensor(void){
    TA0CTL &= MC_0; //Timer A0 stilzetten
    TA1CTL &= MC_0; //Timer A1 stilzetten
}
void startHCSensor(void){
    TA0CTL &= TASSEL_1 + MC_1;                 //Timer 0 weer aanzetten
    TA1CTL &= TASSEL_2 + MC_2 + TACLR + ID_3;        // SMCLK, Cont Mode; start timer
}
// TA1_A1 Interrupt vector
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) TIMER1_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(TA1IV,0x0A))
  {
      case  TA1IV_NONE: break;              // Vector  0:  Geen interrupt
      case  TA1IV_TACCR1:                   // Vector  2:  TACCR1 CCIFG -> Signaal op capture input TA1.1
        if (TA1CCTL1 & CCI)                 // Leest de pin status van capture input TA1.1
        {
            // Opgaande flank gedetecteerd
            if (!Count)
            {
                REdge1 = TA1CCR1;
                Count++;
            }
            else
            {
                Count=0x0;
                distance = ((FEdge-REdge1)/58)/2;
            }

            if (First_Time)
                First_Time = 0x0;
        }
        else
        {
            //Neergaande flank gedetecteerd
            if(!First_Time)
            {
                FEdge = TA1CCR1;
            }
        }
        break;
      case TA1IV_TACCR2: break;             // Vector  4:  TACCR2 CCIFG
      case TA1IV_6: break;                  // Vector  6:  Reserved CCIFG
      case TA1IV_8: break;                  // Vector  8:  Reserved CCIFG
      case TA1IV_TAIFG: break;              // Vector 10:  TAIFG
      default:  break;
  }
}
// TA1_A0 Interrupt vector
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) TIMER1_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (TA1CCTL0 & CCI){                // Leest de pin status van capture input TA1.0

                // Opgaande flank gedetecteerd
                if (!CountS2)
                {
                    REdge1S2 = TA1CCR0;
                    CountS2++;
                }
                else
                {
                    CountS2=0x0;
                    distanceS2 = ((FEdgeS2-REdge1S2)/58)/2; //Berekenen afstand met behulp van de formule uit de hc_sr04 datasheet
                }

                if (First_TimeS2)
                    First_TimeS2 = 0x0;
            }
            else
            {
                // Neergaande flank gedetecteerd
                if(!First_TimeS2)
                {
                    FEdgeS2 = TA1CCR0;
                }
            }
}
// TA0_A0 Interrupt vector
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
    if(toggleSensor){
        // Creëren van start puls voor sensor 1
        P1OUT |= BIT0;
        __delay_cycles(160);
        P1OUT &= ~BIT0;
        toggleSensor = 0;
    }
    else{
        // Creëren van start puls voor sensor 2
        P1OUT |= BIT3;
        __delay_cycles(160);
        P1OUT &= ~BIT3;
        toggleSensor = 1;
    }
}

