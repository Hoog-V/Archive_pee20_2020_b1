/*
 * HC_SR04.c
 *
 *  Created on: Mar 10, 2021
 *      Author: Victor
 *      Code deels gebaseerd op voorbeeld msp430g2xx3_ta_21.c.
 *      Aanpassingen die gemaakt zijn:
 *      - Aangepast voor de MSP430FR2476.
 *      - Aangepast om timer b te gebruiken
 *	- Aangepast om een extra timer te gebruiken om de sensor te kunnen triggeren
 *      - Aangepast om automatisch de lezingen te starten zonder in lpm te gaan
 *      - Aangepast om op 16 mhz te werken -> andere formule -> formule weggehaald en treshold ingesteld
 *      - Aangepast om twee sensoren te kunnen lezen -> weer terug gezet naar 1
 */
#include <msp430fr2476.h> //invoegen voor de registers
#include "HC_SR04.h"
#include "interfaces.h" //Nodig voor de startConditie variabel
unsigned char teller, Eersteflank, tellerS2, EersteflankS2; //Deze variabelen zijn nodig in de capture interrupts van sensor 1 en 2
unsigned int Opgaandeflank1, Neergaandeflank,Opgaandeflank1S2, NeergaandeflankS2, obstakelgedetecteerd, achteruitrijden = 0; //Deze variabelen zijn nodig in de capture interrupt van sensor 2

/*
 * Functie: HC_INIT
 * Description: Initialiseert de pinnen en interrupts voor de botssensoren
 * Parameters: Geen
 * Return: null
 * Geschreven door: Victor
 */
void HC_INIT(){
    //Instellen van de pinnen
    PM5CTL0 &= ~LOCKLPM5;                    // Zorgen dat we de gpio kunnen gebruiken door hoog impedantie modus uit te schakelen
                                             // Dit is specifiek voor de controller die we gebruiken
    P4DIR &= ~BIT7;                                 //P4.7/TB0.1 instellen als ingang
    P4DIR |=  BIT5;                          // P4.5,P4.6 initialiseren voor de trigger signalen en pin van de hc_sr04 sensoren.
    P4OUT &= ~BIT5;                          // P4.5 en P4.6 uitzetten.
    P4SEL1 |= BIT7;                            // P4.7/TB0.1
    // Instellen van timers
    TB0CCTL1 = CAP + CM_3 + CCIE + SCS + CCIS_0; //Timer B capture register 1 instellen om timer capture te doen voor sensor 1.
    TB0CCTL2 = CAP + CM_3 + CCIE + SCS + CCIS_0; //Timer B capture register 2 instellen om timer capture te doen voor sensor 2.
    TB0CTL |= TBSSEL__SMCLK + MC_2 + TBCLR + ID_3;        // SMCLK, Cont Mode; start timer
    TA1CCTL0 = CCIE; //Instellen van de timerinterrupt benodigd om de botssensor te triggeren
    TA1CCR0 = 500-1; //Periode instellen op 8hz -> 32768hz/(8*500) = 8hz
    TA1CTL |= TASSEL__ACLK | ID_3 | MC_1; //ACLK, UP mode; start de timer
    
    // Variabelen de juiste start waarde geven
    teller = 0x0; 
    Eersteflank = 0x01;
    tellerS2 = 0x0;
    EersteflankS2 = 0x01;
    __bis_SR_register(GIE);   //Interrupts aanzetten
}



/*
 * Functie: TIMER0_B1_ISR
 * Description: interrupt routine voor de timer capture van de sensoren
 * Parameters: void
 * Return: null
 * Geschreven door: Victor
 */
 
// TB0_B1 Interrupt vector

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B1_VECTOR))) TIMER0_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{

/* Timer capture voor de botssensoren.
* Noteert de eerste opgaande flank in Opgaandeflank1(S2 voor sensor2) m.b.v. de timer count register.
* Noteert vervolgens de eerst volgende neergaandeflank en slaat het op in Neeergaande flank m.b.v. de timer count register
* Berekent het tijdverschil tussen de twee flanken.
*/
    switch(__even_in_range(TB0IV,TB0IV_TBIFG))
    {
        case TB0IV_NONE:
            break;                               // Geen interrupt
            //TB0IV_TBCCR1 is voor sensor 1
        case TB0IV_TBCCR1:
           //Capture mode wel of niet getriggerd?
            if (TB0CCTL1 & CCI){                // Leest de pin status van capture input TB0.1
                if (!teller)
                {
                    Opgaandeflank1 = TB0CCR1; //Opgaande flank timer count opslaan
                    teller++; //Teller op één zetten zodat hij niet nog een keer een opgaande flank noteert.
                }
                else
                {
                    teller=0x0; //Teller resetten
                    
                    //De threshold is ingesteld met een maximum en een minimum waarde. Dit is om te voorkomen dat als er foute lezing is
                    //de robot niet zomaar achteruit gaat rijden. De startConditie variabele wordt in deze context gebruikt om de sensor 
                    //pas te gebruiken als de robot is gestart met behulp van de startknop.
                    //De threshold is berekend op de volgende manier: 
                    //Datasheet formule: Distance = (pulsewidth)/58
                    //pulsewidth is in uS
                    //Deze timer draait met een frequentie van 2Mhz dat houdt in dat een clockcycle 2 us is. 
                    //Dus de formule die we moeten hebben is: Distance = (pulsewidth)/58/2 = (pulsewidth)/116
                    //Als we de threshold willen berekenen moeten we de formule iets omschrijven:
                    //pulsewidth = Distance*116
                    //Voor de max threshold willen we een afstand van 25cm -> pulsewidth = 116*25=2900 afgerond 3000
                    //Voor de min threshold willen we een afstand van 4cm -> pulsewidth = 116*4=464 afgerond 500
                    //Door de variabele Neergaandeflank van Opgaandeflank1 af te trekken krijgen we onze pulsewidth
                    //De variabele achteruitrijden zorgt ervoor dat de sensor niet triggered terwijl de robot achteruit rijdt
                    if((Neergaandeflank-Opgaandeflank1) < 3000 && startConditie && (Neergaandeflank-Opgaandeflank1) > 500 && !achteruitrijden){
                                           obstakelgedetecteerd =  1; //Obstakelgedetecteerd op 1 zetten zodat er in de main loop verder gehandeld wordt.
                    }
                }

                if (Eersteflank)
                    Eersteflank = 0x0; //Zet Eersteflank op nul zodat de neergaande flank gelezen kan worden.
              }
              else
              {
                  // Neergaande flank gedetecteerd
                  if(!Eersteflank)
                  {
                      Neergaandeflank = TB0CCR1; //Neergaande flank timer count opslaan
                  }
            }
            TB0IV &= ~0x02;
            break;                           
            //TB0IV_TBCCR2 is voor sensor 2; maar die hebben we niet meer :)
        case TB0IV_TBCCR2:
            TB0IV &= ~0x04;
            break;
        default:
            TB0IV &= ~TB0IV;
            break;
    }

}
/*
 * Functie: TIMER_B
 * Description: interrupt routine voor het crëeren van de trig pulsen van de sensoren
 * Parameters: void
 * Return: null
 * Geschreven door: Victor
 */
// TB0_B0 Interrupt vector
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_B (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
/* Dit stukje code stuurt een signaal voor de trigger pin
 * van de botssensoren. 
 */
        // Creëren van start puls voor sensor 1
        P4OUT |= BIT5;
        __delay_cycles(160); //10uS pulsbreedte
        P4OUT &= ~BIT5;
}
