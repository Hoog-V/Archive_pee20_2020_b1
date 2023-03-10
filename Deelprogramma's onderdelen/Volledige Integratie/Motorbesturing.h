/*
 * Motorbesturing.h
 *
 *  Created on: 25 Apr 2021
 *      Author: Kinan
 */

#ifndef MOTORBESTURING_H_
#define MOTORBESTURING_H_
#include <stdbool.h>

void Motoren_setup_Pinnen();
void Timers_setup_PWMs();
void Servo_Rechts_Vooruit();
void Servo_Links_Vooruit();
void Servo_Rechtdoor_Vooruit_en_Achteruit();
void Servo_Rechts_Achteruit();
void Servo_Links_Achteruit();
void Motor_Stop();
void Motor_Vooruit();
void Motor_Achteruit();

extern bool Vooruit;
extern bool Achteruit;
void Rechts_Vooruit_Afslaan();
void Links_Vooruit_Afslaan();
void Rechts_Achteruit_afslaan();
void Links_Achteruit_Afslaan();
void Rechtdoor_Vooruit();
void Rechtdoor_Achteruit();

#endif /* MOTORBESTURING_H_ */
