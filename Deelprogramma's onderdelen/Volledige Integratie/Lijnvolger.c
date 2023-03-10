/*
 * Lijnvolger1.c
 *
 *  Created on: 24 Apr 2021
 *      Author: Raja Ramlal en Kinan Makdissi
 */

#include <msp430fr2476.h>
#include <Lijnvolger.h>
//#include <Motorbesturing.h>
#include <stdint.h>
#include <stdbool.h>

const int8_t S1 = BIT1;
const int8_t S2 = BIT2;
const int8_t S3 = BIT3;
const int8_t S4 = BIT4;
const int8_t S5 = BIT5;
const int8_t S6 = BIT6;

bool Ignore_lijnvolger = false;

char Lijnvolger_richting = 'O'; // char richting van Lijnvolger

/*
 * Functie: Sensoren_Setup_Pinnen
 * Description: Instellen van de pinnen en interrupt routine
 * Parameters: Geen
 * Return: NULL
 * Geschreven door: Kinan Makdissi (regel 56 t/m 62) & Raja Ramlal (regel 63)
 */
void Sensoren_Setup_Pinnen()
{
    P5DIR &= ~(S1 | S2 | S3 | S4 | S5 | S6);            // Sensoren pinnen eerste Achteruit en de laatste drie Vooruit
    P5REN |= S1 | S2 | S3 | S4 | S5 | S6;              // Notitie: BIT6 is middelste sensor van de Achteruit sensoren
    P5OUT &= ~(S1 | S2 | S3 | S4 | S5 | S6);
    P5IE |= S1 | S2 | S3 | S4 | S5 | S6;
    P5IES &= ~(S1 | S2 | S3 | S4 | S5 | S6);        //P5IES = S1 | S2 | S3 | S4 | S5 | S6 <<< voor zwarte lijn detectie
    __enable_interrupt();                             // zet de interrupt routine aan
}

//backup voor servohoek         //Servo_rechts_vooruit -> 2200
                                //Servo_Links_Vooruit -> 3400
                                //Servo_Rechtdoor_Vooruit_en_Achteruit -> 2850
                                //Servo_Rechts_Achteruit -> 3400
                                //Servo_Links_Achteruit -> 2200


/*
 * Functie: Sensoren_logica_Interrupt
 * Description: Interrupt routine voor de lijnvolger
 * Parameters: Geen
 * Return: NULL
 * Geschreven door: Raja Ramlal (13 case statements berekend en toegevoegd) & Kinan Makdissi (Kinan moet dit zelf invullen) >> De logica is samen bedacht. 
 */
#pragma vector = PORT5_VECTOR                           // interrupt toegewezen aan P5

__interrupt void Sensoren_logica_Interrupt(void){        // interrupt variabele
if (Ignore_lijnvolger == false){

    switch (P5IFG)                                  // Switch cases voor P5, cases/soorten kruispunten zijn gesorteerd op hexadecimale waarde zodat de code deze sneller kan runnen
    {
    case 0x0A:                                      // A_kruispunt
        Lijnvolger_richting = 'k';
        P5IFG &= ~(0x0A);                           // P5IFG reset
        break;

    case 0x1A:                                      // A_Hoek rechts rechtdoor
        Lijnvolger_richting = 'z';
        P5IFG &= ~(0x1A);                           // P5IFG reset
        break;

    case 0x2A:                                      // A_TSplitsing
        Lijnvolger_richting = 't';
        P5IFG &= ~(0x2A);                           // P5IFG reset
        break;

    case 0x3A:                                      // A_hoek rechts
        Lijnvolger_richting = 'c';
        P5IFG &= ~(0x3A);                           // P5IFG reset
        break;
    
    case 0x3C:                                      // V_Rechts_aanpassing
        //TA0CCR1 = 2200;
        P5IFG &= ~(0x3C);
        break;
    
    case 0x4A:                                      // A_Hoek links rechtdoor
        Lijnvolger_richting = 'x';
        P5IFG &= ~(0x4A);                           // P5IFG reset
        break;

    case 0x50:                                      // V_kruispunt
        Lijnvolger_richting = 'K';
        P5IFG &= ~(0x50);                           // P5IFG reset
        break;

    case 0x52:                                      // V_hoek_rechts_rechtdoor
        Lijnvolger_richting = 'Z';
        P5IFG &= ~(0x52);                           // P5IFG reset
        break;

    case 0x54:                                      // V_TSplitsing
        Lijnvolger_richting = 'T';
        P5IFG &= ~(0x54);                           // P5IFG reset
        break;

    case 0x56:                                      // V_hoek rechts
        Lijnvolger_richting = 'C';
        P5IFG &= ~(0x56);                           // P5IFG reset
        break;

    case 0x58:                                      // V_hoek links rechtdoor
        Lijnvolger_richting = 'P';
        P5IFG &= ~(0x58);                           // P5IFG reset
        break;
    
    case 0x5A:                                      // V_rechtdoor
        //TA0CCR1 = 2850;
        P5IFG &= ~(0x5A);                           // P5IFG reset
        break;
        
    case 0x5C:                                      // V_hoek links
        Lijnvolger_richting = 'V';
        P5IFG &= ~(0x5C);                           // P5IFG reset
        break;
        
    case 0x66:                                      // V_Links_aanpassing
          //TA0CCR1 = 3400;
          P5IFG &= ~(0x66);
          break;

    case 0x6A:                                      // A_hoek links
        Lijnvolger_richting = 'v';
        P5IFG &= ~(0x6A);                           // P5IFG reset
        break;

    default:
        P5IFG = 0;
        break;
        }
    }
}


