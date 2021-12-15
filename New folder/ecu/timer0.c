/******************************************************************************
 *
 * Module      : Timer 0
 *
 * File Name   : timer0.c
 *
 * Description : source file for timer 0 at Atmega16
 *
 * Author      : shiref ahmed
 *
 *******************************************************************************/

#include "timer0.h"
#include "std_types.h"
#include "common_macros.h" /* To use the macros like SET_BIT */
#include <avr/io.h> /* To use Timer0 Registers */
#include <avr/interrupt.h> /* For Timer0 ISR */

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER0_OVF_vect)
{
	if (g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}

ISR(TIMER0_COMP_vect)
{
	if (g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void Timer0_init(Timer0_ConfigType *ConfigPtr)
{
	switch(ConfigPtr->mode)
	{
	case normal:
		TCNT0 = ConfigPtr->initial_val; // Set Timer initial value to 195

		/* Configure the timer0 control register
		 * 1. Non PWM mode FOC0 = 1
		 * 2. Normal Mode WGM01 = 0 & WGM00 = 0
		 * 3. Normal Mode COM00 = 0 & COM01 = 0
		 * 4. clock selected
		 */
		TCCR0 = (1 << FOC0) | (ConfigPtr->clock);

		TIMSK |= (1 << TOIE0); // Enable Timer0 Overflow Interrupt
		break;

	case compare:
		TCNT0 = ConfigPtr->initial_val; // Set Timer initial value to 0

		OCR0  = ConfigPtr->compare_val; // Set Compare Value

		/* Configure timer0 control register
		 * 1. Non PWM mode FOC0 = 1
		 * 2. CTC Mode WGM01 = 1 & WGM00 = 0
		 * 3. Clear OC0 on compare match so COM00 = 0 & COM01 = 1
		 * 4. clock selected
		 */
		TCCR0 |= (1 << FOC0) | (1 << WGM01) | (ConfigPtr->clock) | (1 << COM01);

		TIMSK |= (1 << OCIE0); // Enable Timer0 Compare Interrupt
		break;
	}
}

void Set_Callback(void (*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}

void Timer0_Deinit(void)
{
	TCCR0 = 0;
}
