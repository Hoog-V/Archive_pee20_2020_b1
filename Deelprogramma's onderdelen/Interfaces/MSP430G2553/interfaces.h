/*
 * interfaces.h
 *
 *  Created on: 10 mrt. 2021
 *      Author: Robbert Berrevoets en Victor Hogeweij
 */

#ifndef INTERFACES_H_
#define INTERFACES_H_

extern int ResetKnop,StartKnop,StopKnop; //geheugen reservering voor flags gebruikt voor het doorgeven van knopactie
//Kleuren struct voor het gebruik van de led.
typedef struct {
    int R;
    int G;
    int B;
 }kleuren;
static const kleuren led = {0x08,0x10,0x20}; //Stelt de pinnen in voor de led.

void zet_led_aan(int pin); //Functieprototype voor het aanzetten van led
void zet_led_uit(int pin); //Functieprototype voor het uitzetten van led
void INIT_interfaces(); //Functieprototype voor het initialiseren van pinnen en instellen van interrupts
void zet_klok_op_16mhz(); //Stelt de klokfrequentie in op 16Mhz
//Functieprototype voor de interrupt functie
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
__interrupt void knoppen(void);
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT2_VECTOR))) Port_2 (void);
#else
#error Compiler not supported!
#endif

#endif /* INTERFACES_H_ */
