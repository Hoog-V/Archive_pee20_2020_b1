/*
 * Author: Victor Hogeweij
 * Project: PEE20_Robot
 *
 * Pin connections:
 *
 * P1.4 UART RX 
 * P1.5 UART TX 
 *
 * MSP        RC522
 * P1.0  <-->  IRQ    (MSP430 Interrupt trigger)
 * P1.1  <-->  SCK    (SPI_CLK)
 * P1.2  <-->  MOSI   (SPI_MOSI)
 * P1.3  <-->  MISO   (SPI MISO)
 * P1.6  <-->  SDA	  (SPI_CS)
 * P1.7  <-->  RST
 * VCC   <-->  VCC
 * GND   <-->  GND
 *
 */

#include <msp430.h>
#include <SPI.h>
#include <Mfrc522.h>
#include <UART.h>

#define RC522_CS_PIN  BIT6 // P1.6
#define RC522_RST_PIN BIT7 // P1.7
#define RC522_IRQ_PIN BIT0 //P1.0

void set_clock_16mhz(){
    FRCTL0 = FRCTLPW | NWAITS_1;            //Fram can't run on 16mhz, to solve this we wait one cycle to reduce clk to 8mhz
    P2SEL0 |= BIT0 | BIT1;                  // P2.0~P2.1: crystal pins
     do
     {
         CSCTL7 &= ~(XT1OFFG | DCOFFG);      // Clear XT1 and DCO fault flag
         SFRIFG1 &= ~OFIFG;
     } while (SFRIFG1 & OFIFG);             // Test oscillator fault flag

    __bis_SR_register(SCG0);                // disable FLL
    CSCTL3 |= SELREF__XT1CLK;               // Set REFO as FLL reference source
    CSCTL1 = DCORSEL_5;                     // DCORSEL=5, DCO Range = 16MHz
    CSCTL2 = FLLD_0 + 487;                  // DCODIV = 16MHz
    __delay_cycles(3);
    __bic_SR_register(SCG0);                // enable FLL
}

/**
 * main.c
 */
int main(void)
{   /* Stop watchdog timer */
    WDTCTL = WDTPW | WDTHOLD;

    PM5CTL0 &= ~LOCKLPM5;                    // Disable the GPIO power-on default high-impedance mode
                                                 // to activate previously configured port settings
    set_clock_16mhz(); //Set the mcu systemclock to 16mhz
    /* Init SPI P1.1, 1.2, 1.3 */
    SPI_begin();

    /* Init RC522 */
    RC522_init(RC522_CS_PIN, RC522_RST_PIN, RC522_IRQ_PIN);

    /* Init UART */
    UART_init();

    /*
     * Setting antenna gain to maximum (7) can read cards up to approximately 5cm.
     * Default can read cards up to approximately 1cm.
     * */
    RC522_setAntennaGain(7);
    RC522_clear_interrupt(); //Sets interrupt in rc522 and clears the interrupt register of the rc522
    while(1){
    	RC522_interrupt_transmit(); //Transmit a bit and clear the fifo buffer
        if(CardDetected){
            int *cardInfo= GetCardInfo(); //Retrieve memory adress for the int array where the color and direction value are stored
            int color = *cardInfo; //Retrieve value from adress
            int direction = *(cardInfo+1); //Retrieve by adding one to the stored memory adress to get the number in index 1 of the int array
            UART_printf("Color value: %c \r\n", color); //Print color value
            UART_printf("Direction: %c \r\n",direction); //Print direction value
            RC522_clear_interrupt(); //Sets interrupt in rc522 and clears the interrupt register
            CardDetected = 0; //Turn off the reading sequence
        }
        __delay_cycles(10000); //Wait 625 us
    }


}
