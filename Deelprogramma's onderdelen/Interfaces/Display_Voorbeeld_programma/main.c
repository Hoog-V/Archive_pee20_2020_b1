//***************************************************************************************
//  MSP430 + PCD8544-based LCD (like Nokia 5110)
// 
//  MSP430x2xx Family User's Guide      : http://www.ti.com/lit/ug/slau144j/slau144j.pdf
//  MSP430G2x53 Data Sheet              : http://www.ti.com/lit/ds/symlink/msp430g2553.pdf
//  PCD8544 Data Sheet (Nokia 5110 MCU) : https://www.sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf
// 
//  My setup:
// 
//         NOKIA 5110 LCD                                               MSP-EXP430G2
//       -----------------                                           -------------------
//      |              GND|<-- Ground ------------------------------|J6     GND         |
//      |               BL|<-- Back-light (not connected)
//      |              VCC|<-- Vcc +3..5V --------------------------|J1.1   VCC         |
//      |                 |
//      |              CLC|<-- Clock -------------------------------|J1.7   P1.5        |
//      |              DIN|<-- Data Input --------------------------|J2.15  P1.7        |
//      |               DC|<-- Data/Command (high/low) -------------|J1.3   P1.1        |
//      |               CE|<-- Chip Enable (active low) ------------|J1.2   P1.0        |
//      |              RST|<-- Reset -------------------------------|J2.16  RST
// 
// 
//  This example is based on the RobG's example : http://forum.43oh.com/topic/1312-nokia-5110-display
//  Changes:
//  - Removed graphics function
//  - Removed unused functions
//  + LCD test runs in the loop
//  + Added some bars animation
// 
//***************************************************************************************

#include <msp430g2553.h>
#include "PCD8544.h"
#include "SPI.h"
#include <stdio.h>
#include "math.h"


#define LCD5110_SCLK_PIN            BIT5
#define LCD5110_DN_PIN              BIT7
#define LCD5110_SCE_PIN             BIT0
#define LCD5110_DC_PIN              BIT1
#define LCD5110_SELECT              P1OUT &= ~LCD5110_SCE_PIN
#define LCD5110_DESELECT            P1OUT |= LCD5110_SCE_PIN
#define LCD5110_SET_COMMAND         P1OUT &= ~LCD5110_DC_PIN
#define LCD5110_SET_DATA            P1OUT |= LCD5110_DC_PIN
#define LCD5110_COMMAND             0
#define LCD5110_DATA                1

// LCD functions declaration

// Sets LCD address (x = 0..83, y = 0..5)
void setAddr(unsigned char xAddr, unsigned char yAddr);
void writeToLCD(unsigned char dataCommand, unsigned char data);
void writeCharToLCD(char c);
void writeStringToLCD(const char *string);
void initLCD();
void clearLCD();
void clearBank(unsigned char bank);


void main(void) {

    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    /*  Default settings after reset:
    *
    *   Source of the Main system clock (MCLK) and sub-main system clock (SMCLK) is
    *   Internal Digitally Controlled Oscillator Clock (DCO).
    *   MCLK is about 1 MHz.
    *   VLO is about 12 kHz.
    *
    *   Code execution begins in less then 2 us.
    *
    */

    P1OUT |= LCD5110_SCE_PIN | LCD5110_DC_PIN;  // Disable LCD, set Data mode
    P1DIR |= LCD5110_SCE_PIN | LCD5110_DC_PIN;  // Set pins to output direction

    // Setup USIB
//    P1SEL |= LCD5110_SCLK_PIN | LCD5110_DN_PIN;
//    P1SEL2 |= LCD5110_SCLK_PIN | LCD5110_DN_PIN;
//
//    UCB0CTL0 |= UCCKPH | UCMSB | UCMST | UCSYNC; // 3-pin, 8-bit SPI master
//    UCB0CTL1 |= UCSSEL_2;               // SMCLK
//    UCB0BR0 |= 0x01;                    // 1:1
//    UCB0BR1 = 0;
//    UCB0CTL1 &= ~UCSWRST;               // clear SW

    SPI_begin();

    __delay_cycles(500000);
    initLCD();

    int c;
    clearLCD();
    long int seconden = 0;
    int sec = 0;
    int min = 0;
    char buf[20];
    //int NFCS = 0;
    int rondjes = 1;
    //float afstand;
    //int meters;
    //int cm;
    //afstand = rondjes * 0.01 * (M_PI * 6.6);
    //meters = round(afstand);
    //cm = (afstand - meters) * 100;
    sprintf(buf, "tijd: %d:%d", min, sec);

    while(1) {
        setAddr(0, 0);
        writeStringToLCD(buf);
        __delay_cycles(1000000);
        clearLCD();
        seconden = seconden + 1;
        min = seconden / 60;
        sec = seconden - (min*60);
        sprintf(buf, "tijd: %d:%d", min, sec);

        }
} // eof main


// LCD functions implementation

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

//    UCB0TXBUF = data;
//    while(!(IFG2 & UCB0TXIFG));
    SPI_transfer(data);
    LCD5110_DESELECT;
}

void initLCD() {
    writeToLCD(LCD5110_COMMAND, PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION);
    writeToLCD(LCD5110_COMMAND, PCD8544_SETVOP | 0x3F);
    writeToLCD(LCD5110_COMMAND, PCD8544_SETTEMP | 0x02);
    writeToLCD(LCD5110_COMMAND, PCD8544_SETBIAS | 0x03);
    writeToLCD(LCD5110_COMMAND, PCD8544_FUNCTIONSET);
    writeToLCD(LCD5110_COMMAND, PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);
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

