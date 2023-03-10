Dit is een programma waar twee HC-SR04 ultrasoon sensoren mee bedient kan worden.
---
## Werking

Het programma creert een UART verbinding met de aangeleverde UART library van school (albeit met een kleine aanpassing). Vervolgens gebruikt hij de ingebouwde timer interrupts van de msp430 om de
tijd tussen de trig en de echo puls te detecteren van de ultrasoon sensoren. Vervolgens berekent hij met een simpele formule de afstand.
De afstand drukt hij af op de UART verbinding met 115200 BAUD.

---
## Aansluitdiagram

      ACLK = LFXT1 = 32kHz crystal ; SMCLK = MCLK = 16 MHz; Baudrate = 115200:
                MSP430FR2476
             -----------------
         /|\|              XIN|-
          | |                 | 32kHz
          --|RST          XOUT|-
            |                 |
            |             P1.4|--> TXD
            |             P1.5|<-- RXD
            |             P4.5|--> HC_SR04 Sensor2 TRIG -->|
            |             P4.6|--> HC_SR04 Sensor1 TRIG -->|
            |       P4.7/TB0.1|<-- HC_SR04 Sensor1 ECHO <-|
            |       P5.0/TB0.2|<-- HC_SR04 Sensor2 ECHO <-|

---
## Controller compatibiliteit

Dit is een versie die compatibel is met de MSP430FR2476 Er is ook een versie voor de MSP430G2553 die staat in de map MSP430G2553 als je een map terug gaat.

---
## Compiler compatibiliteit

Dit programma is zo geschreven dat zowel de Ti compiler als de (ietswat kieskeurigere) gnu gcc compiler gebruikt kan worden. Het programma geeft geen compileer fouten of waarschuwingen.

---
## Bron van code

Dit programma is zelf geschreven. Maar bevat invloeden van de TI voorbeelden die te vinden zijn in resource explorer.
De gebruikte UART library is het enige wat niet helemaal zelf geschreven is.
