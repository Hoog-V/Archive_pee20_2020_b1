#include <msp430.h> 
#include <stdbool.h>
#include <stdint.h>
#include <Motorbesturing.h>
//#include <Lijnvolger.h>
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	Motoren_setup_Pinnen();
	Timers_setup_PWMs();
	__delay_cycles(50000000);
	Motor_Vooruit();
	__delay_cycles(50000000);
	Motor_Achteruit();
	__delay_cycles(50000000);
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
	__delay_cycles(50000000);
	Motor_Stop();
	while(1)
	{

	}

}
