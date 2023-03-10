/*
 * HC_SR04.h
 *
 *  Created on: Mar 10, 2021
 *      Author: victor
 */

#ifndef HC_SR04_H_
#define HC_SR04_H_
void HC_INIT(); //Functieprototype voor het initialiseren van de botssensor
void startHCSensor();//Functieprototype voor het starten van de HC sensor
void stopHCSensor();//Functieprototype voor het stoppen van de HC sensor
unsigned int getDistanceS1(); //Functieprototype voor het ophalen van de sensordata van botssensor 1
unsigned int getDistanceS2(); //Functieprototype voor het ophalen van de sensordata van botssensor 2
#endif /* HC_SR04_H_ */
