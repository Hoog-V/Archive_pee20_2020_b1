#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>
#include <Motor1.h>
#include <Lijnvolger1.h>

//__interrupt void Sensoren_logica_Interrupt(void);
int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    Motoren_setup_Pinnen();
    Timers_setup_PWMs();
    Sensoren_Setup_Pinnen();
    Motor_Vooruit();
    __delay_cycles(50000000);
    Motor_Achteruit();
    __delay_cycles(50000000);
    Motor_Stop();
    Servo_Rechtdoor_Vooruit_en_Achteruit();
    __delay_cycles(50000000);
    Servo_Rechts_Vooruit();
    __delay_cycles(50000000);
    Servo_Links_Vooruit();
    __delay_cycles(50000000);
    Servo_Rechtdoor_Vooruit_en_Achteruit();
    __delay_cycles(50000000);
    Servo_Rechts_Achteruit();
    __delay_cycles(50000000);
    Servo_Links_Achteruit();
//    __enable_interrupt();

    while(1)
    {
//        Motor_Vooruit();
//        __delay_cycles(1000000);
//        Motor_Achteruit();
//        __delay_cycles(1000000);
//        __delay_cycles(50000000);
//        Servo_Rechts_Vooruit();
//        __delay_cycles(50000000);
//        Servo_Rechtdoor_Vooruit_en_Achteruit();
//         __delay_cycles(50000000);
//        Servo_Links_Vooruit();
//        Servo_Rechts_Achteruit();
//        __delay_cycles(1000000);
//        Servo_Links_Achteruit();
//        __delay_cycles(1000000);

//        __delay_cycles(16000000);
//        Servo_Links_Achteruit();
//        __delay_cycles(16000000);
//        Motor_Vooruit();
//        __delay_cycles(16000000);
//        Motor_Achteruit();
//        __delay_cycles(16000000);
//        Motor_Stop();

    }
}
