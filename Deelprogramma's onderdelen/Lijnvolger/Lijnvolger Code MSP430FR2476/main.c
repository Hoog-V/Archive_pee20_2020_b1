/*
 * main.c
 *
 *  Created on: 31 mei 2021
 *      Author: Raja Ramlal
 */

#include <Lijnvolger.h>
#include <Motorbesturing.h>
#include <msp430.h>
/*
 * Functie: Testcode voor de lijnvolger in samenwerking met de motorbesturing
 * Geschreven door: Raja Ramlal
 */
int main (){
    zet_klok_op_16mhz();        //Klok instellen op 16MHz
    Sensoren_Setup_Pinnen();    //Pinnen setup en interrupt enable functie
  //  Motoren_setup_Pinnen();     //Pinnen van de motorbesturing definieren
   // Timers_setup_PWMs();        //Timers instellen voor de motorbesturing

    while(1){
      Lijnvolger_situaties();
    }

}

