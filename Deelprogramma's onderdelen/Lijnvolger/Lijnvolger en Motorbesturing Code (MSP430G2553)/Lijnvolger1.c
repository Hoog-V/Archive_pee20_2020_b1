/*
 * Lijnvolger1.c
 *
 *  Created on: 24 Apr 2021
 *      Author: Kinan
 */

#include <msp430g2553.h>
#include <Lijnvolger1.h>
#include <stdint.h>
#include <stdbool.h>

const int8_t S1 = BIT1;
const int8_t S2 = BIT2;
const int8_t S3 = BIT3;
const int8_t S4 = BIT4;
const int8_t S5 = BIT5;
const int8_t S6 = BIT6;

//bool Vooruit = true;
//bool Achteruit = false;

char Lijnvolger_richting = "O"; // char richting van Lijnvolger
// De hieronder variabeles worden gelijk gesteld aan de main variabele van de lijnvolger.
//char V_Rechtdoor = "R"; //deze hoeft geen char te hebben
//char V_Kruispunt = "K";
//char V_TSplitsing = "T";
//char V_Hoek_Rechts_En_Rechtdoor = "Z";
//char V_Hoek_Links_En_Rechtdoor = "X";
//char V_Hoek_Rechts = "C";
//char V_Hoek_Links = "V";
//
//char A_Rechtdoor = "r"; //deze hoeft geen char te hebben
//char A_Kruispunt = "k";
//char A_TSplitsing = "t";
//char A_Hoek_Rechts_En_Rechtdoor = "z";
//char A_Hoek_Links_En_Rechtdoor = "x";
//char A_Hoek_Rechts = "c";
//char A_Hoek_Links = "v";

void Sensoren_Setup_Pinnen()
{
    P2DIR &= ~(S1 | S2 | S3 | S4 | S5 | S6); // Sensoren pinnen eerste Achteruit en de laatste drie Vooruit
    P2REN |= S1 | S2 | S3 | S4 | S5 | S6; // Notitie: BIT6 is middelste sensor van de Achteruit sensoren
    P2OUT &= ~(S1 | S2 | S3 | S4 | S5 | S6);
    P2IE |= S1 | S2 | S3 | S4 | S5 | S6;
    P2IES &= ~(S1 | S2 | S3 | S4 | S5 | S6);
}

#pragma vector = PORT2_VECTOR
__interrupt void Sensoren_logica_Interrupt(void)

{
    switch (P2IFG)
    {
    case 0x5A:                  // V_rechtdoor
        TA0CCR1 = 2850;
        P2IFG &= ~(0x5A);       // P2IFG reset
        break;

    case 0x3C: // V_Rechts_aanpassing
        TA0CCR1 = 2200;
        P2IFG &= ~(0x3C);
        break;

    case 0x66: // V_Links_aanpassing
        TA0CCR1 = 3400;
        P2IFG &= ~(0x66);
        break;

    case 0x50:  // V_kruispunt
        Lijnvolger_richting = "K";
        P2IFG &= ~(0x50);       // P2IFG reset
        break;

    case 0x52:      // V_hoek rechts rechtdoor
        Lijnvolger_richting = "Z";
        P2IFG &= ~(0x52);       // P2IFG reset
        break;

    case 0x54:                  // V_TSplitsing
        Lijnvolger_richting = "T";
        P2IFG &= ~(0x54);       // P2IFG rese
        break;

    case 0x56:                  // V_hoek rechts
        Lijnvolger_richting = "C";
        P2IFG &= ~(0x56);       // P2IFG reset
        break;

    case 0x58:                  // V_hoek links rechtdoor
        Lijnvolger_richting = "Z";
        P2IFG &= ~(0x58);       // P2IFG reset
        break;

    case 0x5C:                  // V_hoek links
        Lijnvolger_richting = "V";
        P2IFG &= ~(0x5C);       // P2IFG reset
        break;

    case 0x66: // Rechts_Achteruit_aanpassing
        TA0CCR1 = 3400;
        P2IFG &= ~(0x66);
        break;

    case 0x3C: //Links_Achteruit_aanpassing
        TA0CCR1 = 2200;
        P2IFG &= ~(0x3C);
        break;

    case 0x0A:                   // A_kruispunt
        Lijnvolger_richting = "k";
        P2IFG &= ~(0x0A);       // P2IFG reset
        break;

    case 0x1A:                     // A_Hoek rechts rechtdoor
        Lijnvolger_richting = "z";
        P2IFG &= ~(0x1A);       // P2IFG reset
        break;

    case 0x2A:                      // A_TSplitsing
        Lijnvolger_richting = "t";
        P2IFG &= ~(0x2A);       // P2IFG reset
        break;

    case 0x3A:                  // A_hoek rechts
        Lijnvolger_richting = "c";
        P2IFG &= ~(0x3A);       // P2IFG reset
        break;

    case 0x4A:                  // A_Hoek links rechtdoor
        Lijnvolger_richting = "x";
        P2IFG &= ~(0x4A);       // P2IFG reset
        break;

    case 0x6A:                  // A_hoek links
        Lijnvolger_richting = "v";
        P2IFG &= ~(0x6A);       // P2IFG reset
        break;

    default:
        break;
    }
}

