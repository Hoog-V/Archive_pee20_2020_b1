/*
 * Motor1.h
 *
 *  Created on: 24 Apr 2021
 *      Author: Kinan
 */

#ifndef MOTOR1_H_
#define MOTOR1_H_

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



#endif /* MOTOR1_H_ */
