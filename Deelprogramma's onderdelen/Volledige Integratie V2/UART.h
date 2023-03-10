/*
 * UART.h
 *
 *  Created on: Mar 15, 2021
 *      Author: Victor, the one and only
 */

#ifndef UART_H_
#define UART_H_
#include "stdarg.h"

#define RXD BIT5 // RX P1.5
#define TXD BIT4 // TX P1.4

void UART_init(void)
{
    P1SEL0 |= (RXD | TXD);                  // Configure RXD and TXD pins
    // Configure UART
    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 |= UCSSEL__SMCLK;

    // Baud Rate calculation
    // 16000000/(16*115200) = 8.68
    // Fractional portion = 0.68
    // User's Guide Table 17-4: UCBRSx = 0xF7
    // UCBRFx = int ( (52.083-52)*16) = 10
    UCA0BR0 = 8;                             // 16000000/8/115200
    UCA0BR1 = 0x00;
    UCA0MCTLW = 0xF700 | UCOS16 | UCBRF_10;
    UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
}

void UART_putc(unsigned char c)
{
    while(!(UCA0IFG&UCTXIFG));
    UCA0TXBUF = c;                          // TX
}

void UART_puts(const char *str)
{
     while(*str) UART_putc(*str++);
}

static const unsigned long _dv[] = {
//  4294967296      // 32 bit unsigned max
   1000000000,     // +0
    100000000,     // +1
     10000000,     // +2
      1000000,     // +3
       100000,     // +4
//       65535      // 16 bit unsigned max
        10000,     // +5
         1000,     // +6
          100,     // +7
           10,     // +8
            1,     // +9
};

static void _xtoa(unsigned long x, const unsigned long *dp)
{
   char c;
   unsigned long d;
   if(x) {
       while(x < *dp) ++dp;
       do {
           d = *dp++;
           c = '0';
           while(x >= d) ++c, x -= d;
           UART_putc(c);
       } while(!(d & 1));
   } else
       UART_putc('0');
}

static void _puth(unsigned n)
{
   static const char hex[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
   UART_putc(hex[n & 15]);
}

void UART_printf(char *format, ...)
{
   char c;
   int i;
   long n;

   va_list a;
   va_start(a, format);
   while((c = *format++)) {
       if(c == '%') {
           switch(c = *format++) {
               case 's':                       // String
                   UART_puts(va_arg(a, char*));
                   break;
               case 'c':                       // Char
                   UART_putc(va_arg(a, char));
                   break;
               case 'i':                       // 16 bit Integer
               case 'u':                       // 16 bit Unsigned
                   i = va_arg(a, int);
                   if(c == 'i' && i < 0) i = -i, UART_putc('-');
                   _xtoa((unsigned)i, _dv + 5);
                   break;
               case 'l':                       // 32 bit Long
               case 'n':                       // 32 bit uNsigned loNg
                   n = va_arg(a, long);
                   if(c == 'l' &&  n < 0) n = -n, UART_putc('-');
                   _xtoa((unsigned long)n, _dv);
                   break;
               case 'x':                       // 16 bit heXadecimal
                   i = va_arg(a, int);
                   _puth(i >> 12);
                   _puth(i >> 8);
                   _puth(i >> 4);
                   _puth(i);
                   break;
               case 'y':                       // 8 bit heXadecimal
                   i = va_arg(a, int);
                   _puth(i >> 4);
                   _puth(i);
                   break;
               case 0: return;
               default: goto bad_fmt;
           }
       } else
bad_fmt:    UART_putc(c);
   }
   va_end(a);
}


#endif /* UART_H_ */
