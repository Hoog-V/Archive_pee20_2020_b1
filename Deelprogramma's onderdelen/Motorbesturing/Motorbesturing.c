/*
 * Motorbesturing.c
 *
 *  Created on: 25 Apr 2021
 *      Author: Kinan
 */

#include <msp430FR2476.h>
#include <Motorbesturing.h>
#include <stdbool.h>

void Motoren_setup_Pinnen()
{
    P3DIR |= BIT3; // ENABLE DC motoren PWM
    P3SEL0 |= BIT3; // ENABLE DC motoren PWM het was op BIT 2.1
    P4DIR |= BIT0; // SERVO ENABLE PWM (0 geraden)
    P4SEL0 |= BIT0; // SERVO ENABLE PWM (0 geraden)
    P3DIR |= BIT1; //IN1 Vooruit motor1// | BIT2 | BIT3 | BIT4; IN1, IN2, IN3, IN4
    P3DIR |= BIT2; //IN2 Achteruit motor1
    P3OUT |= BIT1;
    P3OUT &= ~BIT2;
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
    //
//    P3DIR |= BIT3; // ENABLE DC motoren PWM
//    P3SEL0 |= BIT3; // ENABLE DC motoren PWM het was op BIT 2.1
//    P4DIR |= BIT0; // SERVO ENABLE PWM (0 geraden)
//    P4SEL0 |= BIT0; // SERVO ENABLE PWM (0 geraden)
//    P3DIR |= BIT1; //IN1 Vooruit motor1// | BIT2 | BIT3 | BIT4; IN1, IN2, IN3, IN4
//    P3DIR |= BIT2; //IN2 Achteruit motor1
//    P3OUT |= BIT1;
//    P3OUT &= ~BIT2;

    // Timer A3 voor de Servo ENABLE //
    TA3CTL |= MC__UP | TASSEL__SMCLK | ID_3 /*| TACLR*/; // Timer SERVO Motor
    TA3CCR0 = 40000; // Servo recht door PWM 50HZ 1.5ms
    TA3CCR1 = 3000;
    TA3CCTL0 = OUTMOD_7;
    TA3CCTL1 = OUTMOD_7;

    // Timer A2 voor de motoren ENABLE //
    TA2CTL = MC__UP | TASSEL__SMCLK | ID_3 /*| TACLR*/; // Timer DC Motor
    TA2CCR0 = 1000; // DC PWM 60% dutycycle en 2000 HZ
    TA2CCR1 = 600;
    TA2CCTL0 = OUTMOD_7;
    TA2CCTL1 = OUTMOD_7;

}
void Servo_Rechts_Vooruit()
{
    TA3CCR1 = 2200;
}
void Servo_Links_Vooruit()
{
    TA3CCR1 = 3400;
}
void Servo_Rechtdoor_Vooruit_en_Achteruit()
{
    TA3CCR1 = 2850;
}
void Servo_Rechts_Achteruit()
{
    TA3CCR1 = 3400;
}
void Servo_Links_Achteruit()
{
    TA3CCR1 = 2200;
}
void Motor_Stop()
{
    P3OUT &= ~BIT1;
    P3OUT &= ~BIT2;
}
void Motor_Vooruit()
{
    bool Vooruit = true;
    bool Achteruit = false;
    P3OUT |= BIT1;
    P3OUT &= ~BIT2;
}
void Motor_Achteruit()
{
    bool Vooruit = false;
    bool Achteruit = true;
    P3OUT &= ~BIT1;
    P3OUT |= BIT2;
}

void Rechts_Vooruit_Afslaan()    //////// Rechts Afslaan Vooruit functie
{
    Motor_Stop();
    Servo_Rechtdoor_Vooruit_en_Achteruit();
    Motor_Achteruit();
    __delay_cycles(100000);
    Motor_Stop();
    Servo_Rechts_Vooruit();
    Motor_Vooruit();
    __delay_cycles(100000);
}

void Links_Vooruit_Afslaan()   //////// Links Afslaan Vooruit functie
{
    Motor_Stop();
    Servo_Rechtdoor_Vooruit_en_Achteruit();
    Motor_Achteruit();
    __delay_cycles(100000);
    Motor_Stop();
    Servo_Links_Vooruit();
    Motor_Vooruit();
    __delay_cycles(100000);
}

void Rechts_Achteruit_afslaan()   //////// Rechts Afslaan Achteruit functie
{
    Motor_Stop();
    Servo_Rechtdoor_Vooruit_en_Achteruit();
    Motor_Vooruit();
    __delay_cycles(100000);
    Motor_Stop();
    Servo_Rechts_Achteruit();
    Motor_Achteruit();
    __delay_cycles(100000);
}

void Links_Achteruit_Afslaan()  //////// Links Afslaan Achteruit functie
{
    Motor_Stop();
    Servo_Rechtdoor_Vooruit_en_Achteruit();
    Motor_Vooruit();
    __delay_cycles(100000);
    Motor_Stop();
    Servo_Links_Achteruit();
    Motor_Achteruit();
    __delay_cycles(100000);
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
