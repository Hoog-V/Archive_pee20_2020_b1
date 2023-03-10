/*
 * interfaces.c
 *
 *  Created on: 10 mrt. 2021
 *  Author: Robbert Berrevoets en Victor Hogeweij
 *  Deze code bevat een paar functies uit de code van RobG : http://forum.43oh.com/topic/1312-nokia-5110-display
 *
 */

#include "interfaces.h"
#include <msp430.h>
#include "PCD8544.h" //Nodig voor het font van het display
#include "SPI.h"  //Nodig voor communicatie interface display
#include "math.h" //Nodig voor modules en floor division operaties


//Variabelen: looptijd, afstand en knopstatus initialiseren en gelijk stellen aan nul
volatile long looptijd = 0; //Hier wordt de verstreken tijd in opgeslagen
volatile long afstand =  0; //Hier wordt de afstand in opgeslagen
volatile char knopstatus = 0; //Hier wordt de laatst ingedrukte knop in opgeslagen


#define LCD5110_SCLK_PIN            BIT1
#define LCD5110_DN_PIN              BIT2
#define LCD5110_SCE_PIN             BIT3
#define LCD5110_DC_PIN              BIT4
#define LCD5110_RST_PIN             BIT1
#define LCD5110_BL_PIN              BIT2
#define LCD5110_SELECT              P4OUT &= ~LCD5110_SCE_PIN
#define LCD5110_DESELECT            P4OUT |= LCD5110_SCE_PIN
#define LCD5110_SET_COMMAND         P4OUT &= ~LCD5110_DC_PIN
#define LCD5110_SET_DATA            P4OUT |= LCD5110_DC_PIN
#define LCD5110_COMMAND             0
#define LCD5110_DATA                1

/*
 * Functie: Huidige_tijd
 * Description: Geeft de huidige (loop)tijd door
 * Parameters: Geen
 * Return: looptijd (unsigned int)
 * Geschreven door: Victor
 */
unsigned int huidige_tijd(){
     return looptijd;
}

/*
 * Functie: zet_led_aan
 * Description: zet de rgb_led aan m.b.v. de gegeven pin
 * Parameters: (int) pin
 * Return: null
 * Geschreven door: Victor
 */
void zet_led_aan(int pin){
    P2OUT |= pin;
}

/*
 * Functie: zet_led_uit
 * Description: zet de rgb_led uit m.b.v. de gegeven pin
 * Parameters: (int) pin
 * Return: null
 * Geschreven door: Victor
 */
void zet_led_uit(int pin){
    P2OUT &= ~pin;
}

/*
 * Functie: zet_klok_op_16mhz
 * Description: zet de systemclock op 16 MHz
 * Parameters: Geen
 * Return: null
 * Geschreven door: Victor
 */
void zet_klok_op_16mhz(){
    PM5CTL0 &= ~LOCKLPM5;                   // Schakel de hoog impedantiemodus van de pinnen uit
                                           // om de pinnen in te kunnen stellen
    FRCTL0 = FRCTLPW | NWAITS_1; 	      //Laat de FRAM op 8 Mhz draaien i.p.v. 16 MHz want fram kan niet op 16MHz draaien
    P2SEL0 |= BIT0 | BIT1;                   // P2.0~P2.1: crystal pins
    __bis_SR_register(SCG0);              // Frequency locked loop uitschakelen om de systeem klok in te kunnen stellen
    CSCTL3 |= SELREF__XT1CLK;            // Gebruik de externe klokkristal als referentie om de FLL in te kunnen stellen
    CSCTL1 = DCORSEL_5;                 // DCORSEL=5, DCO Range = 16MHz
    CSCTL2 = FLLD_0 + 487;             // DCODIV = 16MHz
    __delay_cycles(3);
    __bic_SR_register(SCG0);                   // FLL instellen
    CSCTL4 = SELMS__DCOCLKDIV | SELA__XT1CLK;   // set XT1 (~32768Hz) as ACLK source, ACLK = 32768Hz
                                                 // default DCOCLKDIV as MCLK and SMCLK source
}

/*
 * Functie: INIT_interfaces
 * Description: Stelt de pinnen, RTC en het display in voor gebruik
 * Parameters: Geen
 * Return: null
 * Geschreven door: Robbert en Victor
 */
void INIT_interfaces(){
    /* LED PINNEN instellen
     * en interrupt instellen voor knop pinnen.
     */
    P2DIR |= led.R | led.G | led.B;                             // P2.5, P2.6 en P2.7 als input instellen (V)
    P2OUT &= ~led.R + ~led.G + ~led.B;			              //Register P2OUT op 0 zetten zodat er geen ledjes branden (V)
    P3DIR &= ~BIT5 + ~BIT6 + ~BIT7;                          // P3.5, P3.6 en P3.7 als output instellen (V)
    P3OUT |= BIT5 + BIT6 + BIT7;                            // P3.5, P3.6 en P3.7 als input met pullup instellen (V)
    P3REN |= BIT5 + BIT6 + BIT7;                           // P3.5, P3.6 en P3.7 pullup (V)
    P3IE |=  BIT5 + BIT6 + BIT7;                          // P3.5, P3.6 en P3.7 interrupt instellen (V)
    P3IES |= BIT5 + BIT6 + BIT7;                         // P3.5, P3.6 en P3.7 Hi/lo transitie (V)
    P3IFG &= ~BIT5 + ~BIT6 + ~BIT7;                     // P3.5, P3.6 en P3.7 IFG geleegd (V)
    //
    P4DIR |=  LCD5110_RST_PIN  |  LCD5110_BL_PIN  | LCD5110_SCE_PIN | LCD5110_DC_PIN;  // Set pins to output direction // (R)
    P4OUT |=  LCD5110_RST_PIN   |  LCD5110_BL_PIN  | LCD5110_SCE_PIN | LCD5110_DC_PIN;  // Disable LCD, set Data mode  // (R)

    /* Initialiseren van Real time clock
     *
     * Instellingen:
     * klokbron=XT1CLK(extern kristal van 32khz)
     * RTCIE (Real time clock interrupt) = 1
     * klok_divider (RTCPS)= 1024
     * Periode= (klok_divider)1024/(frequentie)32768 * (RTCMOD)32 = 1 sec
     */
    RTCMOD = 32-1; //RTCMOD instellen op 32 om een periode van 1 sec te krijgen
    RTCCTL = RTCSR | RTCPS__1024 | RTCIE; //klokdivider, RTC resetten en interrupt instellen.
    __bis_SR_register(GIE);                            // interrupts aanzetten
    //Display gereed maken
    SPI_begin();
    writeToLCD(LCD5110_COMMAND, PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION);
    writeToLCD(LCD5110_COMMAND, PCD8544_SETVOP | 0x3F);
    writeToLCD(LCD5110_COMMAND, PCD8544_SETTEMP | 0x02);
    writeToLCD(LCD5110_COMMAND, PCD8544_SETBIAS | 0x03);
    writeToLCD(LCD5110_COMMAND, PCD8544_FUNCTIONSET);
    writeToLCD(LCD5110_COMMAND, PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);
    clearLCD();
}

/*
 * Functie: knoppen
 * Beschrijving: interrupt routine geeft door welke knop er ingedrukt is.
 *               Start de rtc als de startknop is ingedrukt, stopt de rtc als de stopknop ingedrukt
 *               en reset de rtc en het display als de resetknop is ingedrukt.
 * Koppeling aan main loop: m.b.v. de variabele knopstatus:
 *   - Als de knopstatus gelijk is aan D. Startknop ingedrukt
 *   - Als de knopstatus gelijk is aan S. Stopknop ingedrukt
 *   - Als de knopstatus gelijk is aan R. Resetknop ingedrukt
 *
 * Parameters: Geen
 * Return: null
 * Geschreven door: Victor
 */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT3_VECTOR
__interrupt void knoppen(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT3_VECTOR))) Port_3 (void)
#else
#error Compiler not supported!
#endif
{
//Gebruik maken van een switch statement om de pinnen te kunnen onderscheiden m.b.v. de PxIFG register.
switch(_even_in_range(P3IFG,128)) {
    case 0x20: // P3.5(startknop) interrupt
        P3IFG &= ~0x20; //Wist de interrupt flag
        RTCCTL |= RTCSS__XT1CLK;//Start de RTC zodat de klok start met tellen
        knopstatus = 'D'; //Geeft door dat startknop is ingedrukt.
        break;
    case 0x40: // P3.6(stopknop) interrupt
        P3IFG &= ~0x40;
        RTCCTL &= ~RTCSS__XT1CLK; //Stop de RTC zodat de klok stopt met tellen
        knopstatus = 'S';//Geeft door dat stopknop is ingedrukt.
        break;
    case 0x80: // P3.7(resetknop) interrupt
        P3IFG &= ~0x80;
        RTCCTL &= ~RTCSS__XT1CLK; //Stop de RTC zodat de klok stopt met tellen
        looptijd = 0; //tijd resetten
        afstand = 0; //afstand resetten
        Display_nul(); //Afstand,tijd en tags van scherm afhalen
        knopstatus = 'R';//Geeft door dat de resetknop is ingedrukt.
        break;
    default:
        P3IFG = 0;
        break;
}
}

/*
 * Functie: RTC_ISR
 * Beschrijving: interrupt routine van de RTC start om de seconde. Incrementeerd de looptijd
 *               en ververst het display met de nieuwe afstand en tijd
 * Parameters: Geen
 * Return: null
 * Geschreven door: Victor
 */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = RTC_VECTOR
__interrupt void RTC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(RTC_VECTOR))) RTC_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(RTCIV, RTCIV_RTCIF))
    {
        case RTCIV_NONE : break;            // Geen interrupt
        case RTCIV_RTCIF:                   // RTC overflow-> 1 sec is gepasseert
            looptijd++;
            Display_Start();
            break;
        default:
            break;
    }
}

/*
 * Functie: Display_nul
 * Description: Display_Initialize is de functie die gebruikt zal worden in de maincode om de display op te starten.
 * Parameters: Geen
 * Return: null
 * Geschreven door: Robbert
 */
void Display_Initialize(){
    //Brum Initializeren Printen
    setAddr(5, 18);
    writeStringToLCD("Initializing");
    __delay_cycles(200000);
    setAddr(11, 27);
    writeStringToLCD("TURBO BRUM");
    __delay_cycles(5000000);
    clearLCD();
    //Display Printen
    __delay_cycles(500000);
    setAddr(0, 0);
    writeStringToLCD("Tijd: ");
    __delay_cycles(500000);
    setAddr(0, 18);
    writeStringToLCD("Afstand: ");
    __delay_cycles(500000);
    setAddr(0, 36);
    writeStringToLCD("Tags: ");
    __delay_cycles(500000);
    setAddr(56, 9);
    writeStringToLCD("sec");
    __delay_cycles(500000);
     setAddr(56, 27);
     writeStringToLCD("cm");
    __delay_cycles(500000);
    setAddr(56, 45);
    writeStringToLCD("tags");
    __delay_cycles(500000);
}

// Nokia Display Functies.

void setAddr(unsigned char xAddr, unsigned char yAddr) {
    writeToLCD(LCD5110_COMMAND, PCD8544_SETXADDR | xAddr);
    writeToLCD(LCD5110_COMMAND, PCD8544_SETYADDR | yAddr);
}

void writeToLCD(unsigned char dataCommand, unsigned char data) {
    LCD5110_SELECT;

    if(dataCommand) {
        LCD5110_SET_DATA;
    } else {
        LCD5110_SET_COMMAND;
    }
    SPI_transfer(data);
    LCD5110_DESELECT;
}

void writeCharToLCD(char c) {
    unsigned char i;
    for(i = 0; i < 5; i++) {
        writeToLCD(LCD5110_DATA, font[c - 0x20][i]);
    }
    writeToLCD(LCD5110_DATA, 0);
}

void writeStringToLCD(const char *string) {
    while(*string) {
        writeCharToLCD(*string++);
    }
}

void clearLCD() {
    setAddr(0, 0);
    int i = 0;
    while(i < PCD8544_MAXBYTES) {
        writeToLCD(LCD5110_DATA, 0);
        i++;
    }
    setAddr(0, 0);
}


void clearBank(unsigned char bank) {
    setAddr(0, bank);
    int i = 0;
    while(i < PCD8544_HPIXELS) {
        writeToLCD(LCD5110_DATA, 0);
        i++;
    }
    setAddr(0, bank);
}

void clearBankTime(unsigned char bank) {
    setAddr(63, bank);
    int i = 63;
    while(i < PCD8544_HPIXELS) {
        writeToLCD(LCD5110_DATA, 0);
        i++;
    }
    setAddr(0, bank);
}

/*
 * Functie: Display_nul
 * Description: wist de characters van het display voor de afstand,tijd en tags
 * Parameters: Geen
 * Return: null
 * Geschreven door: Robbert
 */
void Display_nul(){
    char null_str[6]={' ',' ',' ',' ',' '};
    setAddr(5, 9); //Stelt de cursor in op x=5,y=9 (locatie van de tijd)
    writeStringToLCD(null_str); //Wist de characters met de null_str
    setAddr(5, 27); //Stelt de cursor in op x=5,y=27 (locatie van de afstand)
    writeStringToLCD(null_str);
}

/*
 * Functie: Display_nul
 * Description: Display_Start wordt telkens in de while loop van de maincode gerunt. Deze update de gegevens op de display.
 * Parameters: Geen
 * Return: null
 * Geschreven door: Robbert
 */
void Display_Start(){
    afstand += 21;
    //variabelen printen
    setAddr(5, 9);
    printInt(looptijd);
    setAddr(5, 27);
    printInt(afstand);
}

void printInt(long getal){
   char buffer[6] = {0,0,0,0,0,0};
   int aantal_getallen = 0;
   long tijdelijk_getal = getal;
   while(tijdelijk_getal > 0){
    tijdelijk_getal /= 10;
    aantal_getallen++;
   }
   unsigned int teller = aantal_getallen;
   while(getal > 0){
      buffer[teller--] = getal%10 + '0';
      getal /= 10;
   }
   writeStringToLCD(&buffer[1]);
}
