/******************************************************************************
 *
 * Module      : DC Motor
 *
 * File Name   : dc_motor.c
 *
 * Description : Source file for DC Motor driver
 *
 * Author      : shiref ahmed
 *
 *******************************************************************************/

#include "dc_motor.h"
#include <avr/io.h>
#include "gpio.h"

void DcMotor_Init(void)
{
	GPIO_setupPinDirection(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, PIN_OUTPUT);
	GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
	GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
}

void DcMotor_Rotate(DcMotor_State state,uint8 speed)
{
	switch (speed) {
	case 0:
		PWM_Timer0_Start(0);
		break;
	case 25:
		PWM_Timer0_Start(64);
		break;
	case 50:
		PWM_Timer0_Start(128);
		break;
	case 75:
		PWM_Timer0_Start(192);
		break;
	case 100:
		PWM_Timer0_Start(255);
		break;
	}

	switch(state){
	case Stop:
		GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
		break;
	case Anti_CW:
		GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_HIGH);
		break;
	case CW:
		GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_HIGH);
		GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
		break;
	}
}

void PWM_Timer0_Start(uint8 duty_cycle)
{
	TCNT0 = 0; //Set Timer Initial value

	OCR0 = duty_cycle; // Set Compare Value

	GPIO_setupPinDirection(MOTOR_EN_PORT_ID, MOTOR_EN_PIN_ID, PIN_OUTPUT);
	//set PB3/OC0 as output pin --> pin where the PWM signal is generated from MC.

	/* Configure timer control register
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */
	TCCR0 = (1 << WGM00) | (1 << WGM01) | (1 << COM01) | (1 << CS01);
}
