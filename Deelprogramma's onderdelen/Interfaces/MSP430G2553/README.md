Dit is een programma waar de LCD, RGB LED en de knoppen in geimplementeerd zijn.
---
##Werking

Het programma bevat code waarmee de knoppen m.b.v. interrupts een flag toggelen die vervolgens in de main loop gebruikt kan worden om dingen aan te sturen. In dit geval een kleur van de RGB led
aanzetten d.m.v. de P1OUT register. Voor het gemak word gebruik gemaakt van een struct zodat we de pinnummers van de led niet hoeven te onthouden om een kleur van een led aan te zetten. Op dit moment
is het lcd nog niet geimplementeerd -> Dit doet Robbert binnenkort.

---
##Aansluitdiagram

           SMCLK = MCLK = 16 MHz;

                 MSP430G2553
             -----------------
         /|\|             P1.3|--> Led Rood
          | |             P1.4|--> Led Groen
          --|RST          P1.5|--> Led Blauw
            |                 |
            |             P?.?|--> Display
            |             P?.?|--> Display
            |             P?.?|--> Display
            |             P?.?|--> Display
            |                 |
            |             P2.3|<-- Start knop
            |             P2.4|<-- Reset knop
            |             P2.5|<-- Stop  knop


---
##Controller compatibiliteit

Dit is een versie die compatibel is met de MSP430G2553. Er is ook een versie voor de MSP430FR2476 die staat in de map MSP430FR2476 als je een map terug gaat.

---
##Compiler compatibiliteit

Dit programma is zo geschreven dat zowel de Ti compiler als de (ietswat kieskeurigere) gnu gcc compiler gebruikt kan worden. Het programma geeft geen compileer fouten of waarschuwingen.

---
##Bron van code

Dit programma is zelf geschreven. Bevat alleen invloeden vanuit de EMS10 lessen.
