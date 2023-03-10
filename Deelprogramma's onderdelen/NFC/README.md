Dit is een programma waar het gebruik van de nfc lezer in gedemonstreerd word
---
## Werking

Het programma creert een UART verbinding met de aangeleverde UART library van school (albeit met een kleine aanpassing). Vervolgens maakt hij verbinding met de nfc lezer en stelt hem in
op interrupt modus. In plaats van te pollen creërt de kaartlezer een interrupt en vraagt de controller aan de kaartlezer om de kaart uittelezen. Dit is veel efficienter dan pollen
en maakt de code integreren een stuk makkelijker. Na het uitlezen van de kaarten drukt de controller de waarde af op de UART verbinding naar de computer.

---
## Aansluitdiagram

      ACLK = LFXT1 = 32kHz crystal ; SMCLK = MCLK = 16 MHz; Baudrate = 115200:
      
                MSP430FR2476
             -----------------
         /|\|              XIN|-
          | |                 | 32kHz
          --|RST          XOUT|-
            |                 |
            |             P1.0|<-- IRQ
			|			  P1.1|--> SPI_SCK
            |             P1.2|<-- SPI_MOSI
            |             P1.3|--> SPI_MISO
            |			  P1.4|--> TXD
            |             P1.5|<-- RXD
            |             P1.6|--> RC522_CS
            |             P1.7|--> RC522_RST

---
## Controller compatibiliteit

Dit is een versie die compatibel is met de MSP430FR2476.

---
## Compiler compatibiliteit

Dit programma is zo geschreven dat het alleen met de TI compiler gecompileerd kan worden.

---
## Bron van code

Dit programma is (deels) zelf geschreven. De meeste code is afkomstig van de NFC voorbeeld van school. Het bevat ook invloeden van de TI voorbeelden die te vinden zijn in resource explorer.
