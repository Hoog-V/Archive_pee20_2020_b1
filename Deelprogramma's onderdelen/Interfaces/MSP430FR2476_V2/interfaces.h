/*
 * interfaces.h
 *
 *  Created on: 10 mrt. 2021
 *      Author: Robbert Berrevoets en Victor Hogeweij
 */

#ifndef INTERFACES_H_
#define INTERFACES_H_

extern volatile char knopstatus; //geheugen reservering voor flags gebruikt voor het doorgeven van knopactie
//Kleuren struct voor het gebruik van de led.
typedef struct {
    int R;
    int G;
    int B;
 }kleuren;
static const kleuren led = {0x20,0x40,0x80}; //Stelt de pinnen in voor de led op pinnen P2.5(R),P2.6(G),P2.7(B).
unsigned int huidige_tijd(); //Functieprototype voor het doorgeven van de looptijd
void zet_led_aan(int pin); //Functieprototype voor het aanzetten van led
void zet_led_uit(int pin); //Functieprototype voor het uitzetten van led
void INIT_interfaces(); //Functieprototype voor het initialiseren van de pinnen, het display en het instellen van interrupts
void zet_klok_op_16mhz(); //Stelt de klok frequentie in op 16mhz
// Instellen van LCD cursor (x = 0..83, y = 0..5)
void setAddr(unsigned char xAddr, unsigned char yAddr);
void writeToLCD(unsigned char dataCommand, unsigned char data);
void writeCharToLCD(char c);
void writeStringToLCD(const char *string);
void clearLCD();
void clearBank(unsigned char bank);
void clearBankTime(unsigned char bank);
void printInt(long number);
void Display_nul();

/* De volgende twee functies zullen gebruikt worden in de mainloop voor de display. */
/* Display_Initialize kan in de main() worden gezet en hoeft dus maar één keer gerunt te worden. */
void Display_Initialize();

/* Display_Start word in de RTC_ISR aangeroepen.
 * In Display_Start word er een variabele afstand gebruikt. dit is de afstand die de wielen afgelegd hebben
 * deze variabele word door de encoder bepaald.*/
void Display_Start();



//Functieprototype voor de interrupt functie voor de knoppen
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
__interrupt void knoppen(void);
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT3_VECTOR))) Port_3 (void);
#else
#error Compiler not supported!
#endif

#endif /* INTERFACES_H_ */
