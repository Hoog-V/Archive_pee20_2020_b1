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

const int8_t S1 = BIT0;
const int8_t S2 = BIT1;
const int8_t S3 = BIT2;
const int8_t S4 = BIT3;
const int8_t S5 = BIT4;
const int8_t S6 = BIT5;
const int8_t S7 = BIT6;
const int8_t S8 = BIT7;

uint8_t Vorige_situatie = 0;

volatile char Lijnvolger_richting = 'D'; // char richting van Lijnvolger

/*
 * Functie: Sensoren_Setup_Pinnen
 * Description: Instellen van de pinnen en interrupt routine
 * Parameters: Geen
 * Return: NULL
 * Geschreven door: Kinan Makdissi (regel 32 t/m 37) & Raja Ramlal (regel 38 & 39)
 */
void Sensoren_Setup_Pinnen()
{
    P5DIR &= ~(S1 | S2 | S3 | S4 | S5 | S6 | S7 | S8);            // Sensoren pinnen eerste Achteruit en de laatste drie Vooruit
    P5REN |= S1 | S2 | S3 | S4 | S5 | S6 | S7 | S8;              // Notitie: BIT6 is middelste sensor van de Achteruit sensoren
    P5OUT &= ~(S1 | S2 | S3 | S4 | S5 | S6 | S7 | S8);
}

/*
 * Functie: Sensoren_logica_Interrupt
 * Description: Interrupt routine voor de lijnvolger
 * Parameters: Geen
 * Return: NULL
 * Geschreven door: Raja Ramlal (13 case statements berekend en toegevoegd) & Kinan Makdissi (Kinan moet dit zelf invullen) >> De logica is samen bedacht. 
 */


void Lijnvolger_situaties(){
    if (P5IN != Vorige_situatie){
        Vorige_situatie = P5IN;

        switch (P5IN)
        {

        case 0x66:                                      // V_rechtdoor
            Lijnvolger_richting = 'D';
            break;

        case 0x0F:                                      // Kruispunt 1
            Lijnvolger_richting = 'K';
            break;

        case 0x6F:                                      // Kruispunt 2
            Lijnvolger_richting = 'M';
            break;

        case 0x6D:                                      // T-Splitsing 1
            Lijnvolger_richting = 'T';
            break;

        case 0x0D:                                      // T-Splitsing 2
            Lijnvolger_richting = 'd';
            break;

        case 0x6E:                                      // Hoek_rechts/rechtdoor 1
            Lijnvolger_richting = 'P';
            break;

        case 0x0E:                                      // Hoek_rechts/rechtdoor 2
            Lijnvolger_richting = 'e';
            break;

        case 0x67:                                      // Hoek_links/rechtdoor 1
            Lijnvolger_richting = 'Z';
            break;

        case 0x07:                                      // Hoek_links/rechtdoor 2
            Lijnvolger_richting = 'f';
            break;

        case 0x65:                                      // Hoek_links 1
            Lijnvolger_richting = 'H';
            break;

        case 0x05:                                      // Hoek_links 2
            Lijnvolger_richting = 'g';
            break;

        case 0x6C:                                      // Hoek_rechts 1
            Lijnvolger_richting = 'I';
            break;

        case 0x0C:                                      // Hoek_rechts 2
            Lijnvolger_richting = 'h';
            break;

        case 0x61:                                      // Vooruit Links Aanpassing 1 sensor 1+6+7
            Lijnvolger_richting = 'V';
            break;

        case 0x68:                                      // Vooruit Rechts Aanpassing 1 sensor  4+6+7
            Lijnvolger_richting = 'C';
            break;

        case 0x81:                                      // Vooruit Links aanpassing 2 sensor 1+8
            Lijnvolger_richting = 'A';
              break;

        case 0x18:                                      // Vooruit Rechts aanpassing 2 sensor 4+5
            Lijnvolger_richting = 'B';
            break;

        case 0x01:                                      // Vooruit Rechts aanpassing 3 sensor 1
            Lijnvolger_richting = 'E';
            break;

        case 0x08:                                      // Vooruit Links aanpassing 3 sensor 4
            Lijnvolger_richting = 'G';
            break;

        case 0x16:                                      // Achteruit Links aanpassing 1 sensor 2+3+5
            Lijnvolger_richting = 'U';
            break;

        case 0x86:                                      //Achteruit Rechts aanpassing 1 sensor 2+3+8
            Lijnvolger_richting = 'W';
            break;
            
        case 0x12:                                        //Achteruit Links aanpassing 2 sensor 2+5
            Lijnvolger_richting = 'J';
            break;

        case 0x82:                                        //Achteruit Rechts aanpassing 2 sensor 2+8
            Lijnvolger_richting = 'N';
            break;
            
        case 0x14:                                        // Achteruit Links aanpassing 3 sensor 3 + 5
            Lijnvolger_richting = 'Q';
            break;

        case 0x84:                                        //Achteruit Rechts aanpassing 3 sensor 3 + 8
            Lijnvolger_richting = 'R';
            break;

        case 0x10:                                        // Achteruit Links aanpassing 3 sensor 5
            Lijnvolger_richting = 'O';
            break;

        case 0x80:                                        //Achteruit Rechts aanpassing 3 sensor 8
            Lijnvolger_richting = 'X';
            break;

        case 0x11:                                        //Achteruit Links aanpassing 4 sensor 1 + 5
            Lijnvolger_richting = 'b';
            break;

        case 0x88:                                        //Achteruit Rechts aanpassing 4 sensor 4 + 8
            Lijnvolger_richting = 'c';
            break;
        }
    }
}
