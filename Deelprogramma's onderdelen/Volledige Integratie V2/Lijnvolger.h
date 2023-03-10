/*
 * Lijnvolger.h
 *
 *  Created on: 31 mei 2021
 *      Author: Raja Ramlal
 */

#include <stdbool.h>
#ifndef LIJNVOLGER_H_
#define LIJNVOLGER_H_

extern volatile char Lijnvolger_richting;
void zet_klok_op_16mhz();
void Sensoren_Setup_Pinnen();
void Lijnvolger_situaties();
extern bool Ignore_lijnvolger;
#endif /* LIJNVOLGER_H_ */
