/******************************************************************************
 *
 * Module      : DC Motor
 *
 * File Name   : dc_motor.h
 *
 * Description : header file for DC Motor driver
 *
 * Author      : shiref ahmed
 *
 *******************************************************************************/

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define MOTOR_IN1_PORT_ID 		PORTC_ID
#define MOTOR_IN1_PIN_ID 		PIN3_ID

#define MOTOR_IN2_PORT_ID 		PORTC_ID
#define MOTOR_IN2_PIN_ID 		PIN4_ID

// Connected with OC0 at ATMEGA16
#define MOTOR_EN_PORT_ID 		PORTB_ID
#define MOTOR_EN_PIN_ID 		PIN3_ID

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/

typedef enum {
	Stop, Anti_CW, CW
}DcMotor_State;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description:
 * Function responsible for motor initialization
 */
void DcMotor_Init(void);

/*
 * Description:
 * Function responsible for working direction & percentage speed of the motor
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed);

/*
 * Description:
 * Function responsible for adjusting compare value and generating PWM signal
 * to change speed for the motor
 */
void PWM_Timer0_Start(uint8 duty_cycle);

#endif /* DC_MOTOR_H_ */
