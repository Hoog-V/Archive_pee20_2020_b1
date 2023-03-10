/*
 * HC_SR04.c
 *
 *  Created on: Mar 10, 2021
 *      Author: Victor
 *      Code deels gebaseerd op voorbeeld msp430g2xx3_ta_21.c.
 *      Aanpassingen die gemaakt zijn:
 *      - Aangepast voor de msp430fr2476.
 *      - Aangepast om timer b te gebruiken
 *      - Aangepast om automatisch de lezingen te starten zonder in lpm te gaan
 *      - Aangepast om op 16 mhz te werken -> andere formule
 *      - Aangepast om twee sensoren te kunnen lezen
 */
#include <msp430fr2476.h>
#include "HC_SR04.h"

int toggleSensor = 0; //Deze variabele wordt gebruikt in de interrupt om te schakelen van sensor.
unsigned char teller, Eersteflank, tellerS2, EersteflankS2; //Deze variabelen zijn nodig in de capture interrupts van sensor 1 en 2
unsigned int Opgaandeflank1, Neergaandeflank,Opgaandeflank1S2, NeergaandeflankS2, distance, distanceS2; //Deze variabelen zijn nodig in de capture interrupt van sensor 2

/*
 * Functie: HC_INIT
 * Description: Initialiseert de pinnen en interrupts voor de botssensoren
 * Parameters: Geen
 * Return: null
 * Geschreven door: Victor
 */
void HC_INIT(){
    //Instellen van de pinnen
    P4DIR &= ~BIT7;                                 //P4.7/TB0.1 instellen als ingang
    P4DIR |=  BIT5 | BIT6;                          // P4.5,P4.6 initialiseren voor de trigger signalen en pin van de hc_sr04 sensoren.
    P4OUT &= ~BIT5 + ~BIT6;                          // P4.5 en P4.6 uitzetten.
    P5DIR &= ~BIT0;                          // P5.0/TB0.2 instellen als ingang
    P4SEL1 |= BIT7;                            // P4.7/TB0.1
    P5SEL1 |= BIT0;                            //en P5.0/TB0.2 instellen als timer ingangen
    // Instellen van timers
    TB0CCTL0 = CCIE;                              // Instellen van timer0 interrupt voor het triggeren van de sensoren
    TB0CCR0 = 62500-1;                              // Instellen van de periode voor de timer0 interrupt
    TB0CCTL1 = CAP + CM_3 + CCIE + SCS + CCIS_0; //Timer B capture register 1 instellen om timer capture te doen voor sensor 1.
    TB0CCTL2 = CAP + CM_3 + CCIE + SCS + CCIS_0; //Timer B capture register 2 instellen om timer capture te doen voor sensor 2.
    TB0CTL |= TBSSEL__SMCLK + MC_2 + TBCLR + ID_3;        // SMCLK, Cont Mode; start timer
    // Variabelen de juiste start waarde geven
    teller = 0x0;
    Eersteflank = 0x01;
    tellerS2 = 0x0;
    EersteflankS2 = 0x01;
    __bis_SR_register(GIE);   //Interrupts aanzetten
}

/*
 * Functie: getDistanceS1
 * Description: Geeft de afstand van sensor 1
 * Parameters: void
 * Return: distance (afstand sensor 1)
 * Geschreven door: Victor
 */
unsigned int getDistanceS1(void){
return distance;
}

/*
 * Functie: getDistanceS2
 * Description: Geeft de afstand van sensor 2
 * Parameters: void
 * Return: distance (afstand sensor 2)
 * Geschreven door: Victor
 */
unsigned int getDistanceS2(void){
return distanceS2;
}

/*
 * Functie: stopHCSensor
 * Description: stopt de lezingen van beide sensoren
 * Parameters: void
 * Return: null
 * Geschreven door: Victor
 */
void stopHCSensor(void){
    TB0CTL = MC_0; //Timer A1 stilzetten
}

/*
 * Functie: stopHCSensor
 * Description: start de lezingen van beide sensoren
 * Parameters: void
 * Return: null
 * Geschreven door: Victor
 */
void startHCSensor(void){
    TB0CTL = TBSSEL__SMCLK + MC_2 + TBCLR + ID_3;        // SMCLK, Cont Mode; start timer
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
* Berekent het tijdverschil tussen de twee flanken. En berekent met de formule uit de hc_sr04 datasheet de afstand.
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
                    distance = ((Neergaandeflank-Opgaandeflank1)/58)/2; //Berekenen van de afstand met behulp van de formule uit de hc_sr04 datasheet.
                    //De formule is iets aangepast. Hij deelt hem nog eens door twee omdat we een 16mhz systemclk hebben met een klok input divider van 8
                    //voor de timer. Dus dat geeft een frequentie van 2mhz en klok perioden van 0,5uS. de formule in de datasheet gaat uit van 1us.
                    //Dus moeten we alles delen door twee.
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
            //TB0IV_TBCCR2 is voor sensor 2
        case TB0IV_TBCCR2:
            if (TB0CCTL2 & CCI){                // Leest de pin status van capture input TB0.2
                if (!tellerS2)
                {
                    Opgaandeflank1S2 = TB0CCR2;
                    tellerS2++;
                }
                else
                {
                    tellerS2=0x0;
                    distanceS2 = ((NeergaandeflankS2-Opgaandeflank1S2)/58)/2; //Berekenen afstand met behulp van de formule uit de hc_sr04 datasheet
                }

                if (EersteflankS2)
                    EersteflankS2 = 0x0;
                }
            else
            {
                // Neergaande flank gedetecteerd
                if(!EersteflankS2)
                {
                    NeergaandeflankS2 = TB0CCR2;
                }
            }
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
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
/* De sensoren kunnen niet tegelijk uitgelezen worden. Daarom is er een toggleSensor flag.
 * Deze flag verandert telkens. Waardoor een andere sensor getriggerd wordt.
 * Dit heeft een groot voordeel. Omdat de frequentie waarmee dit gebeurt 32hz /2 sensoren= 16hz per sensor
 * in tweeën splitst
 */
    if(toggleSensor){
        // Creëren van start puls voor sensor 1
        P4OUT |= BIT6;
        __delay_cycles(160);
        P4OUT &= ~BIT6;
        toggleSensor = 0;
    }
    else{
        // Creëren van start puls voor sensor 2
        P4OUT |= BIT5;
        __delay_cycles(160);
        P4OUT &= ~BIT5;
        toggleSensor = 1;
    }
}
