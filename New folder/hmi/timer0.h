/******************************************************************************
 *
 * Module      : Timer 0
 *
 * File Name   : timer0.h
 *
 * Description : header file for timer 0 at Atmega16
 *
 * Author      : shiref ahmed
 *
 *******************************************************************************/

#ifndef TIMER0_H_
#define TIMER0_H_

#include "std_types.h"

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/

typedef enum {
	no_clk, clk, clk_8, clk_64, clk_256, clk_1024, ext_clk_falling, ext_clk_rising
}Clk_Select;

typedef enum {
	normal, compare
}Timer0_Mode;

typedef struct{
	//void (*Timer0_Mode)(Timer0_ConfigType);
	Timer0_Mode mode;
	uint8 initial_val;
	uint8 compare_val;
	Clk_Select clock;
}Timer0_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description:
 * Function responsible for timer0 initialization & configuration modes.
 */
void Timer0_init(Timer0_ConfigType *ConfigPtr);

/*
 * Description:
 * Function to set the Call Back function address.
 */
void Set_Callback(void (*a_ptr)(void));

/*
 * Description:
 * Function to disable Timer0.
 */
void Timer0_Deinit(void);

#endif /* TIMER0_H_ */
