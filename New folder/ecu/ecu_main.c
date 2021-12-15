/*
 ================================================================================================
 Name        : ECU_main.c

 Author      : shiref ahmed


 
 ================================================================================================
 */

#include "uart.h"
#include "external_eeprom.h"
#include "dc_motor.h"
#include "timer0.h"
#include "gpio.h"
#include <avr/io.h>
#include <util/delay.h> /* For the delay functions */
#include "buzzer.h"

#define READY 0x01
#define NOTREADY 0x00




uint16 Tick=0;

/*
 * Description:
 * Function to calculate numbers of Overflow
 * */
void Timer0_Tick(void);

int main(void)
{
	/*******************************************************************************
	 *                           Initialization Section                            *
	 *******************************************************************************/
	SREG |= (1 << 7); // Enable global interrupt

	uint8 key[7] = {0}, match_check = 0, i = 0, option_key = 0, incorrect_pass = 0;

	// Set configuration for Timer0
	Timer0_ConfigType timer_config = {normal, 0, 0, clk_1024};

	// Set Timer0 Call back function
	Set_Callback(Timer0_Tick);

	// Initialize the UART driver with Baud-rate = 9600 bits/sec
	UART_init(9600);

	// Initialize I2C
	EEPROM_init();

	// Receive the password for the first time
	UART_receiveString(key);

	// Configure buzzer
	Buzzer_init();

	// Initialize DC motor
	DcMotor_Init();

	/*Save the password in EEPROM*/
	for (i = 0; i < 6; i++) {
		EEPROM_writeByte(0x0040 + i, key[i]);
		_delay_ms(10);
	}

	/*******************************************************************************
	 *                             Super Loop Section                              *
	 *******************************************************************************/
	while (1) {

		UART_receiveString(key);/* Take password from the user*/

		option_key = UART_recieveByte(); /*receive what he want (open door,change password*/

		/*if he pressed '+' the door will be opened if the password is right*/
		if (option_key == '+') {

			/*This loop to check the password is the same as in EEPROM*/
			for (i = 0; i < 6; i++) {
				EEPROM_readByte(0x0040 + i, &match_check);
				if (match_check == key[i]) {

				} else {
					break;
				}
			}

			/*************************************************************
			 * if the password is right the C_ECU Send READY for HMI_ECU
			 * and start to calculate the time for opening the door 15s
			 * and start to calculate the time for closing the door 15s
			 *************************************************************/
			if (i == 6) {
				UART_sendByte(READY);


				GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_HIGH);
				GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
				

				Timer0_init(&timer_config);
				while (Tick < 457);
				Timer0_Deinit();
				Tick = 0;

				GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
				GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_HIGH);
				

				Timer0_init(&timer_config);
				while (Tick < 457);
				Timer0_Deinit();
				Tick = 0;

				GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
				GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
				
			}
			/*if the password is not right stop the motor and increment the wrong*/
			else {
				UART_sendByte(NOTREADY); /*Send to HMI_ECU the password is wrong */
				GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
				GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);
				incorrect_pass++;

				/* if the user enter the password wrong three time Stop the system for 1 minute*/
				if (incorrect_pass == 3) {
					Timer0_init(&timer_config);
					Buzzer_ON();
					while (Tick != 1828);
					Timer0_Deinit();

					Buzzer_OFF();
					incorrect_pass = 0;
				}
				Buzzer_ON();
				_delay_ms(500);
				Buzzer_OFF();

			}

		}
		/* if he want to change the password */
		else if (option_key == '-') {
			for (i = 0; i < 6; i++) {
				EEPROM_readByte(0x0040 + i, &match_check);
				if (match_check == key[i]) {

				} else {
					break;
				}
			}
			if (i == 6) {
				UART_sendByte(READY);
				UART_receiveString(key);
				for (i = 0; i < 6; i++) {
					EEPROM_writeByte(0x0040 + i, key[i]);
					_delay_ms(10);
				}

			} else {
				UART_sendByte(NOTREADY);

				GPIO_writePin(MOTOR_IN1_PORT_ID, MOTOR_IN1_PIN_ID, LOGIC_LOW);
				GPIO_writePin(MOTOR_IN2_PORT_ID, MOTOR_IN2_PIN_ID, LOGIC_LOW);

			    incorrect_pass++;

				if (incorrect_pass == 3) {
					Timer0_init(&timer_config);
					Buzzer_ON();
					while (Tick != 1828);
					Timer0_Deinit();
					Buzzer_OFF();
					incorrect_pass = 0;
				}
				Buzzer_ON();
				_delay_ms(500);
				Buzzer_OFF();

			}

		}

	}
}

void Timer0_Tick(void){
	Tick++;
}

