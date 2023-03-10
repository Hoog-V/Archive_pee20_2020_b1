/*
 * Motor1.c
 *
 *  Created on: 24 Apr 2021
 *      Author: Kinan
 */
#include <msp430g2553.h>
#include <Motor1.h>
#include <stdbool.h>


void Motoren_setup_Pinnen()
{
    P2DIR |= BIT1; // ENABLE DC motoren PWM
    P2SEL |= BIT1; // ENABLE DC motoren PWM het was op BIT 2.1
    P1DIR |= BIT6; // SERVO ENABLE PWM (0 geraden)
    P1SEL |= BIT6; // SERVO ENABLE PWM (0 geraden)
    P1DIR |= BIT7; //IN1 Vooruit motor1//
    P2DIR |= BIT7; //IN2 Achteruit motor1
    P1OUT |= BIT7; //Begint vooruit
    P2OUT &= ~BIT7; // Achteruit disabled tot botsdetectie
    P2SEL &= ~(BIT0 | BIT2 | BIT6 | BIT7);
    P2SEL2 &= ~(BIT6 | BIT0 | BIT2 | BIT7);
}
void Timers_setup_PWMs()
{
    DCOCTL = 0;
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;
    // Timer A0 voor de Servo PWM ENABLE //
    TA0CTL |= MC_1 | TASSEL_2 | ID_3;
    TA0CCR0 = 40000;
    TA0CCR1 = 2850;
    TA0CCTL0 = OUTMOD_7;
    TA0CCTL1 = OUTMOD_7;
    // Timer A1 voor de dc motoren ENABLE //
    TA1CTL = MC_1 | TASSEL_2 | ID_3;
    TA1CCR0 = 1000; // 60% duty cycle PWM
    TA1CCR1 = 600;
    TA1CCTL0 = OUTMOD_7;
    TA1CCTL1 = OUTMOD_7;

}
void Servo_Rechts_Vooruit()
{
    TA0CCR1 = 3400;
}
void Servo_Links_Vooruit()
{
    TA0CCR1 = 2200;
}
void Servo_Rechtdoor_Vooruit_en_Achteruit()
{
    TA0CCR1 = 2850;
}
void Servo_Rechts_Achteruit()
{
    TA0CCR1 = 2200;
}
void Servo_Links_Achteruit()
{
    TA0CCR1 = 3400;
}
void Motor_Stop()
{
    P1OUT &= ~BIT7;
    P2OUT &= ~BIT7;
}
void Motor_Vooruit()
{
    bool Vooruit = true;
    bool Achteruit = false;
    P1OUT |= BIT7;
    P2OUT &= ~BIT7;

}
void Motor_Achteruit()
{
    bool Vooruit = false;
    bool Achteruit = true;
    P1OUT &= ~BIT7;
    P2OUT |= BIT7;
}

