Dit is een programma waar de HC-SR04 ultrasoon sensor mee bedient kan worden.
---
## Werking

Het programma creert een UART verbinding met de aangeleverde UART library van school (albeit met een kleine aanpassing). Vervolgens gebruikt hij de ingebouwde timer interrupts van de msp430 om de
tijd tussen de trig en de echo puls te detecteren. Vervolgens berekent hij met een simpele formule de afstand. De afstand drukt hij vervolgens af op de UART verbinding met 115200 BAUD.

---
## Aansluitdiagram

	ACLK = LFXT1 = 32 kHz crystal ; SMCLK = MCLK = 16 MHz;

                 MSP430G2553
             -----------------
         /|\|              XIN|-
          | |                 | 32kHz
          --|RST          XOUT|-
            |                 |
            |             P1.0|--> HC_SR04 Sensor1 TRIG -->|
            |             P1.3|--> HC_SR04 Sensor2 TRIG -->|
            |       P2.0/TA1.0|<-- HC_SR04 Sensor2 ECHO <-|
            |       P2.1/TA1.1|<-- HC_SR04 Sensor 1 ECHO <-|

---
## Controller compatibiliteit

Dit is een versie die compatibel is met de MSP430G2553. Er is ook een versie voor de MSP430FR2476 die staat in de map MSP430FR2476 als je een map terug gaat.

---
## Compiler compatibiliteit

Dit programma is zo geschreven dat zowel de Ti compiler als de (ietswat kieskeurigere) gnu gcc compiler gebruikt kan worden. Het programma geeft geen compileer fouten of waarschuwingen.

---
## Bron van code

Dit programma is zelf geschreven. Maar bevat invloeden van de TI voorbeelden die te vinden zijn in resource explorer.
De gebruikte UART library is het enige wat niet helemaal zelf geschreven is.
