/*
 * buzzer.h
 *
 *  Author: shiref ahmed
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"

#define BUZZER_PORT_ID 	PORTC_ID
#define BUZZER_PIN_ID 	PIN3_ID

void Buzzer_init(void);

void Buzzer_ON(void);

void Buzzer_OFF(void);

#endif /* BUZZER_H_ */
