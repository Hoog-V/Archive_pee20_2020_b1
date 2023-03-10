/*
 * Programmeurs: Raja Ramlal, Robbert Berrevoets, Victor Hogeweij, Kinan Makdissi
 * Project: PEE20_Robot
 * Integratiecode Encoder, Interfaces, NFC, Botsdetectie, Motorbesturing & Lijnvolger
 */

#include <msp430.h>
#include <SPI.h>
#include <Mfrc522.h>
#include <stdbool.h>
#include "interfaces.h"
#include "HC_SR04.h"
#include "Motorbesturing.h"
#include "Lijnvolger.h"

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
//    UART_init();

    HC_INIT();
    P5DIR |= BIT7;
    /*
     * Instellen antenna gain om kaarten op afstand van 5cm te kunnen lezen
     * Op de standaard instelling kan er alleen kaarten tot 1cm gelezen worden
     * */
    RC522_setAntennaGain(7);
    INIT_interfaces(); //initialiseert de interfaces
    Display_Initialize();

    //Motorbesturing initialiseren
    Motoren_setup_Pinnen();
    Timers_setup_PWMs();

    //Lijnvolger initialiseren
    Sensoren_Setup_Pinnen();    //Pinnen setup en interrupt enable functie

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
        //Ignore_lijnvolger = false;     //Beginnen met lezen input lijnvolger

        while(CardDetected){
                        TA0CTL = 0;
                        RC522_interrupt_transmit();
                        RC522_clear_interrupt();            //Sets interrupt in rc522 and clears the interrupt register
                        int *cardInfo= GetCardInfo();       //Retrieve memory adress for the int array where the color and direction value are stored
                        NFCKleur= *cardInfo;                //Retrieve value from adress
                        NFCRichting = *(cardInfo+1);        //Retrieve by adding one to the stored memory adress to get the number in index 1 of the int array
                        int NFCID = *(cardInfo+2);
//                        UART_printf("Color value: %c \r\n",NFCKleur); //Print color value
//                        UART_printf("Direction: %c \r\n", NFCRichting); //Print direction value
                        if(NFCID != prevNFCCard){
                        prevNFCCard = NFCID;
                        switch(NFCKleur){
                        case 'R':
                            tags[0] += 1;
                            zet_led_uit(led.R);
                            zet_led_uit(led.G);
                            zet_led_uit(led.B);
                            zet_led_aan(led.R);
                            NFCKleur = 0;
                            break;
                        case 'G':
                            tags[1] += 1; //--> (tags[1]++)
                            zet_led_uit(led.R);
                            zet_led_uit(led.G);
                            zet_led_uit(led.B);
                            zet_led_aan(led.G);
                            NFCKleur = 0;
                            break;
                        case 'B':
                            tags[2] += 1;
                            zet_led_uit(led.R);
                            zet_led_uit(led.G);
                            zet_led_uit(led.B);
                            zet_led_aan(led.B);
                            NFCKleur = 0;
                            break;
                        }
                        }
                        NFCKleur = 0;
                        RC522_clear_interrupt(); //Sets interrupt in rc522 and clears the interrupt register
                        TA0CTL = TASSEL_1 | ID_3 | MC_1;
                        CardDetected = 0; //Turn off the reading sequence

                        }
        //Print alleen de afstand tussen sensor1 en het object kleiner is dan 30.
//        if(getDistanceS1() < 15 || getDistanceS1() < 15){
//            P5OUT |= BIT7;
//            if (Vooruit){
//                Vooruit = false;        //Achteruit Rijden na obstakeldetectie
//                }
//
//            else{
//                Vooruit = true;
//            }
//
//            if (Achteruit){
//                    Achteruit = false;  //Vooruit rijden na obstakeldetectie
//                }
//
//            else{
//                Achteruit = true;
//            }
//        }


        //Integratie motorbesturing, lijnvolger en NFC richting opdracht
        if (Vooruit){

            if (Lijnvolger_richting == 'K'){             //V_Kruispunt
                Ignore_lijnvolger = true;               //Lijnvolger input negeren
                switch (NFCRichting){
                    case 'D':
                        Rechtdoor_Vooruit();            //Rechtdoor NFC opdracht uitvoeren
                        break;

                    case 'R':
                        Rechts_Vooruit_Afslaan();       //Rechtsaf NFC opdracht uitvoeren
                        break;

                    case 'L':
                        Links_Vooruit_Afslaan();        //Linksaf NFC opdracht uitvoeren
                        break;
                    }
                    Lijnvolger_richting = 0;            //Lijnvolger 
              }

            if (Lijnvolger_richting == 'T'){            //V_TSplitsing
                Ignore_lijnvolger = true;
                switch (NFCRichting){
                    case 'R':
                        Rechts_Vooruit_Afslaan();
                        break;

                    case 'L':
                        Links_Vooruit_Afslaan();
                        break;
                    
                    default:
                        Rechts_Vooruit_Afslaan();           //else rechts
                        break;
                    }
                    Lijnvolger_richting = 0;
              }

            //Sturen met servo op Hoek rechts/rechtdoor met NFC Richting opdracht
            if (Lijnvolger_richting == 'Z'){            // V_hoek_rechts_rechtdoor
                Ignore_lijnvolger = true;
                switch (NFCRichting){
                    case 'D':
                        Rechtdoor_Vooruit();
                        break;

                    case 'R':
                        Rechts_Vooruit_Afslaan();
                        break;
                    
                    default:
                        Rechtdoor_Vooruit();           //else vooruit vervangt NFC opdracht Linksaf
                        break;
                    }
                    Lijnvolger_richting = 0;
            }

            if (Lijnvolger_richting == 'P'){            // V_hoek links rechtdoor
                Ignore_lijnvolger = true;
                switch (NFCRichting){
                    case 'D':
                        Rechtdoor_Vooruit();
                        break;

                    case 'L':
                        Links_Vooruit_Afslaan();
                        break;
                    
                    default:
                        Rechtdoor_Vooruit();           //else vooruit vervangt NFC opdracht Rechtsaf
                        break;
                    }
                    Lijnvolger_richting = 0;
              }

            if (Lijnvolger_richting == 'C'){            // V_hoek_rechts
                Ignore_lijnvolger = true;
                Rechts_Vooruit_Afslaan();
                Lijnvolger_richting = 0;
            }

            if (Lijnvolger_richting == 'V'){           // V_hoek_links
                Ignore_lijnvolger = true;
                Links_Vooruit_Afslaan();
                Lijnvolger_richting = 0;
            }
        }


        ///////////////////////////////////////////////////////////////
        if (!Vooruit){                  //ACHTERUIT RIJDEN

            if (Lijnvolger_richting == 'k'){            // A_kruispunt
                Ignore_lijnvolger = true;
                switch (NFCRichting){
                    case 'D':
                        Rechtdoor_Achteruit();
                        break;

                    case 'R':
                        Rechts_Achteruit_afslaan();
                        break;

                    case 'L':
                        Links_Achteruit_Afslaan();
                        break;
                    }
                Lijnvolger_richting = 0;
            }

            if (Lijnvolger_richting == 't'){            // A_TSplitsing
                Ignore_lijnvolger = true;
                switch (NFCRichting){
                    case 'R':
                        Rechts_Achteruit_afslaan();
                        break;

                    case 'L':
                        Links_Achteruit_Afslaan();
                        break;
                    
                    default:
                        Rechts_Achteruit_afslaan();           //else rechts vervangt NFC opdracht Rechtdoor
                        break;
                    }
                Lijnvolger_richting = 0;
            }

            if (Lijnvolger_richting == 'z'){            //A_Hoek rechts rechtdoor
                Ignore_lijnvolger = true;
                switch (NFCRichting){
                    case 'D':
                        Rechtdoor_Achteruit();
                        break;

                    case 'R':
                        Rechts_Achteruit_afslaan();
                        break;
                        
                    default:
                        Rechtdoor_Achteruit();           //else Rechtdoor vervangt NFC opdracht Linksaf
                        break;
                    }
                Lijnvolger_richting = 0;
            }

            if (Lijnvolger_richting == 'x'){            // A_Hoek links rechtdoor
                Ignore_lijnvolger = true;
                switch (NFCRichting){
                    case 'D':
                        Rechtdoor_Achteruit();
                        break;

                    case 'L':
                        Links_Achteruit_Afslaan();
                        break;
                        
                    default:
                        Rechtdoor_Achteruit();           //else Rechtdoor vervangt NFC opdracht Rechtsaf
                        break;
                    }
                Lijnvolger_richting = 0;
            }

            if (Lijnvolger_richting == 'c'){            // A_hoek_rechts
                Ignore_lijnvolger = true;
                Rechts_Achteruit_afslaan();
                Lijnvolger_richting = 0;
            }

            if (Lijnvolger_richting == 'v'){           // A_hoek_links
                Ignore_lijnvolger = true;
                Links_Achteruit_Afslaan();
                Lijnvolger_richting = 0;
            }
        }

    }
}
