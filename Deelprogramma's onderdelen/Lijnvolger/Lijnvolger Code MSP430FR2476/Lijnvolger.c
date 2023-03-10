/*
 * Lijnvolger1.c
 *
 *  Created on: 24 Apr 2021
 *      Author: Raja Ramlal en Kinan Makdissi
 */

#include <msp430fr2476.h>
#include <Lijnvolger.h>
#include <Motorbesturing.h>
#include <stdint.h>
#include <stdbool.h>

const int8_t S1 = BIT1;
const int8_t S2 = BIT2;
const int8_t S3 = BIT3;
const int8_t S4 = BIT4;
const int8_t S5 = BIT5;
const int8_t S6 = BIT6;

volatile char Lijnvolger_richting = 'D'; // char richting van Lijnvolger


/*
 * Functie: zet_klok_op_16mhz
 * Description: System clock op 16 MHz
 * Parameters: Geen
 * Return: NULL
 * Geschreven door: Victor Hogeweij
 */
void zet_klok_op_16mhz(){
    PM5CTL0 &= ~LOCKLPM5;                              // Schakel de hoog impedantiemodus van de pinnen uit
                                                      // om de pinnen in te kunnen stellen
    FRCTL0 = FRCTLPW | NWAITS_1;                     //Laat de FRAM op 8 Mhz draaien i.p.v. 16 MHz want fram kan niet op 16MHz draaien
    P2SEL0 |= BIT0 | BIT1;                          // P2.0~P2.1: crystal pins

__bis_SR_register(SCG0);                            // Frequency locked loop uitschakelen om de systeem klok in te kunnen stellen
    CSCTL3 |= SELREF__XT1CLK;                      // Gebruik de externe klokkristal als referentie om de FLL in te kunnen stellen
    CSCTL1 = DCORSEL_5;                           // DCORSEL=5, DCO Range = 16MHz
    CSCTL2 = FLLD_0 + 487;                       // DCODIV = 16MHz
__delay_cycles(3);
__bic_SR_register(SCG0);                                 // FLL instellen
    CSCTL4 = SELMS__DCOCLKDIV | SELA__XT1CLK;           // set XT1 (~32768Hz) as ACLK source, ACLK = 32768Hz
                                                       // default DCOCLKDIV as MCLK and SMCLK source
}

/*
 * Functie: Sensoren_Setup_Pinnen
 * Description: Instellen van de pinnen en interrupt routine
 * Parameters: Geen
 * Return: NULL
 * Geschreven door: Kinan Makdissi (regel 56 t/m 62) & Raja Ramlal (regel 62 & 63)
 */
void Sensoren_Setup_Pinnen()
{
    P5DIR &= ~(S1 | S2 | S3 | S4 | S5 | S6);            // Sensoren pinnen eerste Achteruit en de laatste drie Vooruit
    P5REN |= S1 | S2 | S3 | S4 | S5 | S6;              // Notitie: BIT6 is middelste sensor van de Achteruit sensoren
    P5OUT &= ~(S1 | S2 | S3 | S4 | S5 | S6);
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

void Lijnvolger_situaties(){
    switch (P5IN)
    {
    case 0x18:                                      // V_Rechts_aanpassing
        //TA0CCR1 = 2200;
        Lijnvolger_richting = 'A';
        P5IFG &= ~(0x3C);
        break;
    
    case 0x24:                                      // V_rechtdoor
        //TA0CCR1 = 2850;
        Lijnvolger_richting = 'D';
        P5IFG &= ~(0x5A);                           // P5IFG reset
        break;

    case 0x2E:                                      // V_kruispunt
        Lijnvolger_richting = 'K';
        P5IFG &= ~(0x50);                           // P5IFG reset
        break;

    case 0x2C:                                      // V_hoek rechts rechtdoor
        Lijnvolger_richting = 'Z';
        P5IFG &= ~(0x52);                           // P5IFG reset
        break;

    case 0x2A:                                      // V_TSplitsing
        Lijnvolger_richting = 'T';
        P5IFG &= ~(0x54);                           // P5IFG reset
        break;

    case 0x28:                                      // V_hoek rechts
        Lijnvolger_richting = 'C';
        P5IFG &= ~(0x56);                           // P5IFG reset
        break;

    case 0x26:                                      // V_hoek links rechtdoor
        Lijnvolger_richting = 'Z';
        P5IFG &= ~(0x58);                           // P5IFG reset
        break;
    
    case 0x42:                                      // V_Links_aanpassing
          //TA0CCR1 = 3400;
        Lijnvolger_richting = 'a';
          P5IFG &= ~(0x66);
          break;

    case 0x22:                                      // V_hoek links
        Lijnvolger_richting = 'V';
        P5IFG &= ~(0x5C);                           // P5IFG reset
        break;


    case 0x74:                                      // A_kruispunt
        Lijnvolger_richting = 'k';
        P5IFG &= ~(0x0A);                           // P5IFG reset
        break;

    case 0x64:                                      // A_Hoek rechts rechtdoor
        Lijnvolger_richting = 'z';
        P5IFG &= ~(0x1A);                           // P5IFG reset
        break;

    case 0x54:                                      // A_TSplitsing
        Lijnvolger_richting = 't';
        P5IFG &= ~(0x2A);                           // P5IFG reset
        break;

    case 0x44:                                       // A_hoek rechts
        Lijnvolger_richting = 'c';
        P5IFG &= ~(0x3A);                           // P5IFG reset
        break;

    case 0x34:                                       // A_Hoek links rechtdoor
        Lijnvolger_richting = 'x';
        P5IFG &= ~(0x4A);                           // P5IFG reset
        break;

    case 0x14:                                      // A_hoek links
        Lijnvolger_richting = 'v';
        P5IFG &= ~(0x6A);                           // P5IFG reset
        break;

    default:
        P5IFG = 0;
        break;
    }
}


