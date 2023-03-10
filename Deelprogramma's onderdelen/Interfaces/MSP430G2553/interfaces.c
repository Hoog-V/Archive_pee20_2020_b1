/*
 * interfaces.c
 *
 *  Created on: 10 mrt. 2021
 *      Author: Robbert Berrevoets en Victor Hogeweij
 */
#include "interfaces.h"
#include <msp430.h>
int ResetKnop,StartKnop,StopKnop = 0;

//Functie om led aan te zetten
void zet_led_aan(int pin){
    P1OUT |= pin;
}

//Functie om led uit te zetten.
void zet_led_uit(int pin){
    P1OUT &= ~pin;
}


void INIT_interfaces(){
    P1DIR |= BIT3 + BIT4 + BIT5;                               // P1.3, P1.4 en P1.5 als output instellen.
    P1OUT = 0;                                                //Register P2OUT op 0 zetten zodat er geen ledjes branden.
    P2DIR &= ~BIT3 + ~BIT4 + ~BIT5;                          // P2.3, P2.4 en P2.5 als input instellen
    P2OUT |= BIT3 + BIT4 + BIT5;                            // P2.3, P2.4 en P2.5 als input met pullup instellen
    P2REN |= BIT3 + BIT4 + BIT5;                           // P2.3, P2.4 en P2.5 pullup
    P2IE |=  BIT3 + BIT4 + BIT5;                          // P2.3, P2.4 en P2.5 interrupt instellen
    P2IES |= BIT3 + BIT4 + BIT5;                         // P2.3, P2.4 en P2.5 Hi/lo transitie
    P2IFG &= BIT3 + BIT4 + BIT5;                        // P2.3, P2.4 en P2.5 IFG geleegd
    __bis_SR_register(GIE);                            // interrupts aanzetten
}

// Port 2 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void knoppen(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT2_VECTOR))) Port_2 (void)
#else
#error Compiler not supported!
#endif
{
//Omdat bij de MSP430G2553 er maar één vector is voor pin interrupts.
//wordt er gebruik gemaakt van een switch statement om de pinnen te kunnen onderscheiden.
switch(_even_in_range(P2IFG,16)) {
    case 0x08: // P2IFG.3
        P2IFG &= ~0x08; //Wist de interrupt flag
        StartKnop = 1; //Geeft door dat startknop is ingedrukt.
        break;
    case 0x10: //P2IFG.4
        P2IFG &= ~0x10;
        ResetKnop = 1;
        break;
    case 0x20: // P2IFG.5
        P2IFG &= ~0x20;
        StopKnop = 1;
        break;
    default:
        P2IFG = 0;
        break;
}
}

