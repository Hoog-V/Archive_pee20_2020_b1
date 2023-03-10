/*
 * Programmeurs: Raja Ramlal, Robbert Berrevoets, Victor Hogeweij
 * Project: PEE20_Robot
 * Integratiecode Encoder, Interfaces en NFC
 */

#include <msp430.h>
#include <SPI.h>
#include <Mfrc522.h>
#include <UART.h>
#include "interfaces.h"

#define RC522_CS_PIN  BIT6 // P1.6
#define RC522_RST_PIN BIT7 // P1.7
#define RC522_IRQ_PIN BIT0 // P1.0

int prevNFCCard=0; //Opslagplek voor vorige NFC kaart id
/**
 * main.c
 */
int main(void)
{   /* Stop watchdog timer */
    WDTCTL = WDTPW | WDTHOLD;

    zet_klok_op_16mhz();
    /* Initialiseer SPI P1.1, 1.2, 1.3 */
    SPI_begin();

    /* Initialiseer RC522 */
    RC522_init(RC522_CS_PIN, RC522_RST_PIN, RC522_IRQ_PIN);

    /* Initialiseer UART (alleen voor debugging) */
    UART_init();

    /*
     * Instellen antenna gain om kaarten op afstand van 5cm te kunnen lezen
     * Op de standaard instelling kan er alleen kaarten tot 1cm gelezen worden
     * */
    RC522_setAntennaGain(7);
    INIT_interfaces(); //initialiseert de interfaces
    Display_Initialize();
    UART_init();                              //Opzetten van de UART communicatie
    HC_INIT();                                //Opzetten van de HC_SR04
    while(1){
        /*
         * Testfunctie. Test of de led werkt en of de
         * communicatie tussen de main loop en de interrupt functie van de knoppen
         * goed werkt.
         *
         * Als de startknop(P3.5) is ingedrukt gaat de rode led branden
         * Als de stopknop(P3.6) is ingedrukt gaat de groene led branden
         * Als de resetknop(P3.7) is ingedrukt gaat de blauwe led branden
         *
         * Geschreven door: Victor
         */
        switch(knopstatus){
        case 'D':
            zet_led_uit(led.R); //Zet alle leds uit
            zet_led_uit(led.G);
            zet_led_uit(led.B);
            zet_led_aan(led.R); //Zet de rode led aan
//            knopstatus = 0; //reset de knopstatus zodat hij niet blijft draaien
            break;
        case 'S':
            zet_led_uit(led.R);
            zet_led_uit(led.G);
            zet_led_uit(led.B);
            zet_led_aan(led.G);
//            knopstatus = 0;
            break;
        case 'R':
            zet_led_uit(led.R);
            zet_led_uit(led.G);
            zet_led_uit(led.B);
            zet_led_aan(led.B);
//            knopstatus = 0;
            break;
        }
        while(CardDetected){
                        TA2CTL = 0;
                        RC522_interrupt_transmit();
                        RC522_clear_interrupt(); //Sets interrupt in rc522 and clears the interrupt register
                        int *cardInfo= GetCardInfo(); //Retrieve memory adress for the int array where the color and direction value are stored
                        NFCKleur= *cardInfo; //Retrieve value from adress
                        NFCRichting = *(cardInfo+1); //Retrieve by adding one to the stored memory adress to get the number in index 1 of the int array
                        int NFCID = *(cardInfo+2);
                        UART_printf("Color value: %c \r\n",NFCKleur); //Print color value
                        UART_printf("Direction: %c \r\n", NFCRichting); //Print direction value
                        if(NFCID != prevNFCCard){
                        prevNFCCard = NFCID;
                        switch(NFCKleur){
                        case 'R':
                            tags[0] += 1;
                            NFCKleur = 0;
                            break;
                        case 'G':
                            tags[1] += 1; //--> (tags[1]++)
                            NFCKleur = 0;
                            break;
                        case 'B':
                            tags[2] += 1;
                            NFCKleur = 0;
                            break;
                        }
                        }
                        NFCKleur = 0;
                        RC522_clear_interrupt(); //Sets interrupt in rc522 and clears the interrupt register
                        TA2CTL = TASSEL_1 | ID_3 | MC_1;
                        CardDetected = 0; //Turn off the reading sequence

                        }

        __delay_cycles(1000);
    }
}

