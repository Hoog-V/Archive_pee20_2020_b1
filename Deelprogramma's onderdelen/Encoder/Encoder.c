#include <msp430.h> 

int rondjes = 0; // variabele rondjes aangemaakt voor encoder en communicatie met display

#pragma vector = PORT1_VECTOR
__interrupt void Flank_Encoder(void)
{
  rondjes++;
  P1IFG &= ~P1IFG;
}


/*
Geschreven door: Raja Ramlal
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	

void INIT_Encoder(){
	P1DIR |= BIT1;    // P1.1 output
	P1OUT |= 1<<1;    //zorgen dat encoder voeding krijgt P1.1
	P1DIR &= ~(1<<0); //ingang maken van P1.0

	//Interrupt instellen voor P1.0
	P1IES &= ~(1<<0); //P1IES Flank laag naar hoog.
    P1IE |= 1<<0;     //P1IE Interrupt enable.
}

	__enable_interrupt();

	while(1)
	{

	}



	return 0;
}
