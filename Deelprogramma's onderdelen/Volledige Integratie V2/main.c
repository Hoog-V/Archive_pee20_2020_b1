/*
 * Programmeurs: Raja Ramlal, Robbert Berrevoets, Victor Hogeweij, Kinan Makdissi
 * Project: PEE20_Robot
 * Integratiecode Encoder, Interfaces, NFC, Botsdetectie, Motorbesturing & Lijnvolger
 */

#include <msp430.h>
#include "HC_SR04.h" //Voor de botsdetectie
#include <SPI.h>     //Voor de communicatie tussen het display en de NFC lezer
#include <Mfrc522.h> //Voor de NFC lezer
#include <stdbool.h>
#include "interfaces.h" //Voor het display,knoppen en de encoder

#include "Motorbesturing.h" //Voor de motor en de servo
#include "Lijnvolger.h"     //Voor de lijnvolger

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
    PM5CTL0 &= ~LOCKLPM5; //Hoog impedantie modus van de pinnen uitschakelen (alleen voor deze msp430)
    zet_klok_op_16mhz(); //Systemklok instellen op 16MHz

    /* Initialiseer SPI P1.1, 1.2, 1.3 */
    SPI_begin();

    /* Initialiseer RC522 */
    RC522_init(RC522_CS_PIN, RC522_RST_PIN, RC522_IRQ_PIN);
    /* initialiseer de botsdetectie */
    HC_INIT();


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
    Sensoren_Setup_Pinnen();                        //Pinnen setup functie
    Servo_Rechts_Vooruit();
    __delay_cycles(8000000);
    Servo_Rechtdoor_Vooruit_en_Achteruit();
    __delay_cycles(800000);

    while(1){


        //Ignore_Lijnvolger = false;     //Beginnen met lezen input lijnvolger
        Lijnvolger_situaties(); //Uitlezen van de lijnvolger sensoren



        if (obstakelgedetecteerd && startConditie && Vooruit && !achteruitrijden){                  //ACHTERUIT RIJDEN
              Motor_Achteruit();
              //Lijnvolger_richting = 0;
              achteruitrijden = 1; //achteruitrijden op 1 zetten zodat de botsdetectie geen lezingen doet tijdens het rijden
              Vooruit = false; //Zorgen dat de achteruitcases gelezen worden van de lijnvolger
              obstakelgedetecteerd = 0; //obstakelgedetecteerd resetten naar 0. zodat er geen foute lezing komt
          }


        while(CardDetected && startConditie){
                        TA0CTL = 0;
                        RC522_interrupt_transmit();
                        RC522_clear_interrupt();
                        int *cardInfo= GetCardInfo();       //Ophalen geheugenadres waar de kaartkleur en kaartrichting staan
                        NFCKleur= *cardInfo;                //Ophalen kleurwaarde van geheugenadres
                        NFCRichting = *(cardInfo+1);        //Ophalen door 1 toetevoegen om index 1 van de array te krijgen om vervolgens de richting te krijgen
                        int NFCID = *(cardInfo+2);          //Ophalen door 2 toetevoegen om index 2 van de array te krijgen om vervolgens het NFC ID te krijgen
                        //Als de kaart hetzelfde is als de vorig gelezen...Niks doen!
                        if(NFCID != prevNFCCard){
                        prevNFCCard = NFCID; //Opslaan van de huidige NFCID in oude NFC id
                        //De juiste led aanzetten m.b.v. de uitgelezen kleurwaarde
                        switch(NFCKleur){
                        case 'R':
                            tags[0] += 1; //Het aantal rode tags updaten op het scherm
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
                        NFCKleur = 0; //NFCKleur op 0 zetten, voor de zekerheid dat hij niet nog een keer triggered
                        RC522_clear_interrupt(); //Instellen van interrupt register op de RC522 en bestaande interrupts resetten
                        TA0CTL = TASSEL_1 | ID_3 | MC_1; //De timer voor de interrupt trigger routine starten
                        CardDetected = 0; //Zet cardDetected op 0 zodat dit zooitje niet opnieuw draait
                        if (NFCRichting == 'S'){
                            //Als er de richting stop uitgelezen wordt
                            Display_Start();
                            Motor_Stop();
                            RTCCTL &= ~RTCSS__XT1CLK;       //Stop de RTC zodat de klok stopt met tellen
                            NFCRichting = 0;
                         }
                        else{
                            Motor_Vooruit();
                        }
                        }



        //Integratie motorbesturing, lijnvolger en NFC richting opdracht
        if(Vooruit && !obstakelgedetecteerd && !Ignore_Lijnvolger && startConditie){
             switch(Lijnvolger_richting){

                //Rechtdoor
                case 'D':                                       //Recht rjden over de lijn, voor- en achteruit
                    Servo_Rechtdoor_Vooruit_en_Achteruit();
                    Lijnvolger_richting = 0;
                    break;
                
                // Bijsturen op de lijn met meerdere situaties
                case 'A':
                     Servo_BijsturenLinks_Vooruit();           // Links_aanpassing2
                     Lijnvolger_richting = 0;
                    break;
                
                case 'B':                                       // Rechts_aanpassing2
                     Servo_BijsturenRechts_Vooruit();
                     Lijnvolger_richting = 0;
                    break;
                
                case 'C':
                    Servo_BijsturenRechts_Vooruit();            // Rechts_Aanpassing1
                    Lijnvolger_richting = 0;
                    break;
                               
                case 'V':
                    Servo_BijsturenLinks_Vooruit();             // Links_Aanpassing1
                    Lijnvolger_richting = 0;
                    break;
                
                case 'E':
                     Servo_BijsturenLinks_Vooruit();           // Links_aanpassing3
                     Lijnvolger_richting = 0;
                    break;
                
                case 'G':
                     Servo_BijsturenRechts_Vooruit();           // Links_aanpassing3
                     Lijnvolger_richting = 0;
                    break;

                case 'b':
                     Servo_BijsturenRechts_Achteruit();         // Rechts aanpassing 4
                     Lijnvolger_richting = 0;
                     break;

                case 'c':
                     Servo_BijsturenLinks_Achteruit();          // Links aanpassing 4
                     Lijnvolger_richting = 0;
                     break;
                
                //Kruispunt 1
                case 'K':
                   switch (NFCRichting){
                       case 'R':
                           Rechts_Vooruit_Afslaan();           // Default/NFC opdracht naar rechts
                           //NFCRichting = 0;
                           Lijnvolger_richting = 0;
                           break;
                
                       case 'L':
                           Links_Vooruit_Afslaan();             //Linksaf NFC opdracht uitvoeren
                           //NFCRichting = 0;
                           Lijnvolger_richting = 0;
                           break;

                       default:
                           Rechtdoor_Vooruit();                 //Rechtdoor NFC opdracht uitvoeren
                           //NFCRichting = 0;
                           Lijnvolger_richting = 0;
                           break;
                       }
                    break;
                
                //Kruispunt 2
                case 'M':
                   switch (NFCRichting){
                       case 'R':
                           Rechts_Vooruit_Afslaan();           // Default/NFC opdracht naar rechts
                           //NFCRichting = 0;
                           Lijnvolger_richting = 0;
                           break;
                
                       case 'L':
                           Links_Vooruit_Afslaan();             //Linksaf NFC opdracht uitvoeren
                           //NFCRichting = 0;
                           Lijnvolger_richting = 0;
                           break;

                       default:
                           Rechtdoor_Vooruit();                 //Rechtdoor NFC opdracht uitvoeren
                           //NFCRichting = 0;
                           Lijnvolger_richting = 0;
                           break;
                       }
                    break;

                // T-Splitsing 1
                case 'T':
                    switch (NFCRichting){
                        case 'L':
                            Links_Vooruit_Afslaan();
                            //NFCRichting = 0;
                            Lijnvolger_richting = 0;
                            break;

                        default:
                            Rechts_Vooruit_Afslaan();           // Default rechts
                            //NFCRichting = 0;
                            Lijnvolger_richting = 0;
                            break;
                            }
                    break;

                // T-Splitsing 2
                case 'd':
                    switch (NFCRichting){
                        case 'L':
                            Links_Vooruit_Afslaan();
                            //NFCRichting = 0;
                            Lijnvolger_richting = 0;
                            break;
                
                        default:
                            Rechts_Vooruit_Afslaan();           // Default rechts
                            //NFCRichting = 0;
                            Lijnvolger_richting = 0;
                            break;
                            }
                    break;
                
                // Hoek_links/rechtdoor 1
                case 'Z':
                    switch (NFCRichting){
                        case 'L':
                            Links_Vooruit_Afslaan();
                            //NFCRichting = 0;
                            Lijnvolger_richting = 0;
                            break;

                        default:
                            Rechtdoor_Vooruit();           // Default case
                            //NFCRichting = 0;
                            Lijnvolger_richting = 0;
                            break;
                        }
                    break;

                // Hoek_links/rechtdoor 2
                case 'f':
                    switch (NFCRichting){
                        case 'L':
                            Links_Vooruit_Afslaan();
                            //NFCRichting = 0;
                            Lijnvolger_richting = 0;
                            break;
                
                        default:
                            Rechtdoor_Vooruit();           // Default case
                            //NFCRichting = 0;
                            Lijnvolger_richting = 0;
                            break;
                        }
                    break;
                
                // Hoek_rechts/rechtdoor 1
                case 'P':
                    switch (NFCRichting){
                        case 'R':
                            Rechts_Vooruit_Afslaan();
                            Lijnvolger_richting = 0;
                            break;

                        default:
                            Rechtdoor_Vooruit();           // Default case
                            Lijnvolger_richting = 0;
                            break;
                        }
                    break;

                // Hoek_rechts/rechtdoor 2
                case 'e':
                    switch (NFCRichting){
                        case 'R':
                            Rechts_Vooruit_Afslaan();
                            Lijnvolger_richting = 0;
                            break;
                
                        default:
                            Rechtdoor_Vooruit();           // Default case
                            Lijnvolger_richting = 0;
                            break;
                        }
                    break;

                // Hoek Rechts 1
                case 'I':
                    Rechts_Vooruit_Afslaan();
                    Lijnvolger_richting = 0;
                    break;

                // Hoek Rechts 2
                case 'h':
                    Rechts_Vooruit_Afslaan();
                    Lijnvolger_richting = 0;
                    break;

                // Hoek Links 1
                case 'H':
                    Links_Vooruit_Afslaan();
                    Lijnvolger_richting = 0;
                    break;

                // Hoek Links 2
                case 'g':
                    Links_Vooruit_Afslaan();
                    Lijnvolger_richting = 0;
                    break;
             }
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        //Achteruit Rijden
        if (!Vooruit && !obstakelgedetecteerd && !Ignore_Lijnvolger && startConditie && achteruitrijden){
            switch(Lijnvolger_richting){
               
                case 'D':                                       //Recht rijden over de lijn, voor- en achteruit
                       Servo_Rechtdoor_Vooruit_en_Achteruit();
                       Lijnvolger_richting = 0;
                       break;
        
               //Kruispunt 1
                case 'K':
                    Motor_Vooruit();
                    switch (NFCRichting){
                          case 'L':
                              Motor_Stop();
                              Links_Vooruit_Afslaan();             //Linksaf NFC opdracht uitvoeren
                              //NFCRichting = 0;
                              Lijnvolger_richting = 0;
                              break;

                          default:
                              Motor_Vooruit();
                              __delay_cycles(8000000);
                              Rechts_Vooruit_Afslaan();           // Default/NFC opdracht naar rechts
                              //NFCRichting = 0;
                              Lijnvolger_richting = 0;
                              break;
                          }
                Vooruit = true;
                achteruitrijden = 0;
                break;

                //Kruispunt 2
                case 'M':
                    Motor_Vooruit();
                    switch(NFCRichting){
                        case 'D':
                           Rechtdoor_Vooruit();                 //Rechtdoor NFC opdracht uitvoeren
                           //NFCRichting = 0;
                           Lijnvolger_richting = 0;
                           break;

                         case 'L':
                             Links_Vooruit_Afslaan();             //Linksaf NFC opdracht uitvoeren
                             //NFCRichting = 0;
                             Lijnvolger_richting = 0;
                             break;

                         default:
                             Motor_Vooruit();
                             __delay_cycles(3000000);
                             Rechts_Vooruit_Afslaan();           // Default/NFC opdracht naar rechts
                             //NFCRichting = 0;
                             Lijnvolger_richting = 0;
                             break;
                         }
                Vooruit = true;
                achteruitrijden = 0;
                break;

// T-splitsing is hetzelfde als een kruispunt 2: 'M'. Vandaar dat deze hier niet terug is te vinden.

                // Hoek_links/rechtdoor 1
                case 'Z':
                    Motor_Vooruit();
                    switch (NFCRichting){
                        case 'L':
                            Links_Vooruit_Afslaan();
                            //NFCRichting = 0;
                            Lijnvolger_richting = 0;
                            break;

                        default:
                            Links_Vooruit_Afslaan();           // Default case
                            //NFCRichting = 0;
                            Lijnvolger_richting = 0;
                            break;
                        }
                    Vooruit = true;
                    achteruitrijden = 0;
                    break;

                // Hoek_links/rechtdoor 2
                case 'f':
                    Motor_Vooruit();
                    switch (NFCRichting){
                        case 'L':
                            Links_Vooruit_Afslaan();
                            //NFCRichting = 0;
                            Lijnvolger_richting = 0;
                            break;

                        default:
                            Links_Vooruit_Afslaan();           // Default case
                            //NFCRichting = 0;
                            Lijnvolger_richting = 0;
                            break;
                        }
                    Vooruit = true;
                    achteruitrijden = 0;
                    break;

                // Hoek_rechts/rechtdoor 1
                case 'P':
                    Motor_Vooruit();
                    switch (NFCRichting){
                        case 'R':
                            Rechts_Vooruit_Afslaan();
                            Lijnvolger_richting = 0;
                            break;

                        default:
                            Rechts_Vooruit_Afslaan();        // Default case
                            Lijnvolger_richting = 0;
                            break;
                        }
                    Vooruit = true;
                    achteruitrijden = 0;
                    break;

                // Hoek_rechts/rechtdoor 2
                case 'e':
                    Motor_Vooruit();
                    switch (NFCRichting){
                        case 'R':
                            Rechts_Vooruit_Afslaan();
                            Lijnvolger_richting = 0;
                            break;

                        default:
                            Rechts_Vooruit_Afslaan();        // Default case
                            Lijnvolger_richting = 0;
                            break;
                        }
                    Vooruit = true;
                    achteruitrijden = 0;
                    break;

               case 'U':
                    Servo_BijsturenRechts_Achteruit();          // Achteruit Rechts aanpassing 1
                    Lijnvolger_richting = 0;
                    break;

               case 'W':
                    Servo_BijsturenLinks_Achteruit();         // Achteruit Links aanpassing 1
                    Lijnvolger_richting = 0;
                    break;

               case 'J':
                    Servo_BijsturenRechts_Achteruit();          // Achteruit Rechts aanpassing 2
                    Lijnvolger_richting = 0;
                    break;

               case 'N':
                    Servo_BijsturenLinks_Achteruit();          // Achteruit Links aanpassing 2
                    Lijnvolger_richting = 0;
                    break;

               case 'O':
                    Servo_BijsturenRechts_Achteruit();          // Achteruit Rechts aanpassing 3
                    Lijnvolger_richting = 0;
                    break;

               case 'X':
                    Servo_BijsturenLinks_Achteruit();         // Achteruit Links aanpassing 3
                    Lijnvolger_richting = 0;
                    break;

               case 'R':
                    Servo_BijsturenRechts_Achteruit();          // Achteruit Rechts aanpassing 3
                    Lijnvolger_richting = 0;
                    break;

               case 'Q':
                    Servo_BijsturenLinks_Achteruit();         // Achteruit Links aanpassing 3
                    Lijnvolger_richting = 0;
                    break;

               case 'b':
                    Servo_BijsturenLinks_Achteruit();          // Achteruit Rechts aanpassing 4
                    Lijnvolger_richting = 0;
                    break;

               case 'c':
                    Servo_BijsturenRechts_Achteruit();         // Achteruit Links aanpassing 4
                    Lijnvolger_richting = 0;
                    break;

               case 'E':
                    Servo_BijsturenRechts_Achteruit();          // Achteruit Rechts aanpassing 3
                    Lijnvolger_richting = 0;
                    break;

               case 'G':
                    Servo_BijsturenLinks_Achteruit();         // Achteruit Links aanpassing 3
                    Lijnvolger_richting = 0;
                    break;
            }
        }
    }
}
