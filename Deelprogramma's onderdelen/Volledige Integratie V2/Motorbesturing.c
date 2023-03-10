/*
 * Motorbesturing.c
 *
 *  Created on: 25 Apr 2021
 *      Author: Kinan
 */

#include <msp430fr2476.h>
#include <Motorbesturing.h>
#include <stdbool.h>

bool Vooruit = true;
bool Achteruit = false;
bool Ignore_Lijnvolger = false;
int timer = 0;

void Motoren_setup_Pinnen()
{
    P3DIR |= BIT3; // ENABLE DC motoren PWM
    P3SEL0 |= BIT3; // ENABLE DC motoren PWM het was op BIT 2.1
    P4DIR |= BIT0; // SERVO ENABLE PWM (0 graden)
    P4SEL0 |= BIT0; // SERVO ENABLE PWM (0 graden)
    P3DIR |= BIT1; //IN1 Vooruit motor1// | BIT2 | BIT3 | BIT4; IN1, IN2, IN3, IN4
    P3DIR |= BIT2; //IN2 Achteruit motor1
}
void Timers_setup_PWMs()
{
    FRCTL0 = FRCTLPW | NWAITS_1;
    PM5CTL0 &= ~LOCKLPM5;
    __bis_SR_register(SCG0);
    CSCTL1 |= SELREF__XT1CLK;
    CSCTL1 = DCORSEL_5; // 16MHZ
    CSCTL2 = FLLD_0 + 487;
    __delay_cycles(3);
    __bic_SR_register(SCG0);
    CSCTL4 = SELMS__DCOCLKDIV | SELA__XT1CLK;

    // Timer A3 voor de Servo ENABLE //
    TA3CTL |= MC__UP | TASSEL__SMCLK | ID_3 /*| TACLR*/; // Timer SERVO Motor
    TA3CCR0 = 40000; // Servo recht door PWM 50HZ 1.5ms
    TA3CCR1 = 3000;
    TA3CCTL0 = OUTMOD_7;
    TA3CCTL1 = OUTMOD_7;

    // Timer A2 voor de motoren ENABLE //
    TA2CTL = MC__UP | TASSEL__SMCLK | ID_3 /*| TACLR*/; // Timer DC Motor
    TA2CCR0 = 100;                                      // DC PWM 45% dutycycle en 2000 HZ
    TA2CCR1 = 81;                                       //81
    TA2CCTL0 = OUTMOD_7;
    TA2CCTL1 = OUTMOD_7;
}

void Motor_Stop()
{
    P3OUT &= ~BIT1;
    P3OUT &= ~BIT2;
}

void Motor_Vooruit()
{
    P3OUT |= BIT1;
    P3OUT &= ~BIT2;
}

void Motor_Achteruit()
{
    P3OUT &= ~BIT1;
    P3OUT |= BIT2;
}


void Servo_Rechtdoor_Vooruit_en_Achteruit()
{
    TA3CCR1 = 2396; //2396
}

void Servo_BijsturenRechts_Vooruit()
{
    TA3CCR1 = 2200; //2200
}

void Servo_BijsturenLinks_Vooruit()
{
    TA3CCR1 = 2600; //2600
}

void Servo_BijsturenRechts_Achteruit()
{
    TA3CCR1 = 2550;
}

void Servo_BijsturenLinks_Achteruit()
{
    TA3CCR1 = 2250;
}


void Servo_Rechts_Vooruit()
{
    TA3CCR1 = 1450;
}

void Servo_Links_Vooruit()
{
    TA3CCR1 = 3350;
}

void Rechtdoor_Vooruit()      //////// Rechtdoor Vooruit functie
{
    Servo_Rechtdoor_Vooruit_en_Achteruit();
    Motor_Vooruit();
}

void Rechtdoor_Achteruit()    //////// Rechtdoor Achteruit functie
{
    Servo_Rechtdoor_Vooruit_en_Achteruit();
    Motor_Achteruit();
}

void Rechts_Vooruit_Afslaan()           // Rechts Afslaan Vooruit functie MET 8.24V
{
    Ignore_Lijnvolger = true;
    Motor_Stop();
    Servo_Rechtdoor_Vooruit_en_Achteruit();
    Motor_Achteruit();
    __delay_cycles(20000000);
    Motor_Stop();
    Servo_Rechts_Vooruit();
    Motor_Vooruit();
    __delay_cycles(40000000);
    Ignore_Lijnvolger = false;
}

void Links_Vooruit_Afslaan()   //////// Links Afslaan Vooruit functie MET 8.08V
{
    Ignore_Lijnvolger = true;
    Motor_Stop();
    Servo_Rechtdoor_Vooruit_en_Achteruit();
    Motor_Achteruit();
    __delay_cycles(15000000);
    Motor_Stop();
    Servo_Links_Vooruit();
    Motor_Vooruit();
    __delay_cycles(41000000);
    Ignore_Lijnvolger = false;
}
