/*
 ================================================================================================
 Name        : HMI_main.c

 Author      : shiref ahmed

 

 ================================================================================================
 */

#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timer0.h"
#include <avr/io.h> // to use SREG
#include <util/delay.h> /* For the delay functions */

#define READY 0x01




uint16 Tick=0;

/*
 * Description:
 * Function to calculate numbers of Overflow
 * */
void Timer0_Tick(void);

void main_options(void)
{
	LCD_clearScreen();
	LCD_displayString("+ :Open Door");
	LCD_moveCursor(1,0);
	LCD_displayString("- :Change Pass");
}

void door_unlock(void)
{
	LCD_clearScreen();
	LCD_displayString("Door is ");
	LCD_moveCursor(1, 0);
	LCD_displayString("unlocking");
}

void door_lock(void)
{
	LCD_clearScreen();
	LCD_displayString("Door is Locking");
}



void first_run(uint8 *key){
	LCD_clearScreen();
	uint8 i = 0, check_num = 0;
	LCD_displayString("Please Enter New");
	LCD_moveCursor(1, 0);
	LCD_displayString("Password:");
	for (i = 0; i < 5; i++) {
		check_num = KEYPAD_getPressedKey();
		if (check_num <= 9 && check_num >= 0) {
			key[i] = check_num;
			LCD_displayCharacter('*');
			_delay_ms(300);
		} else {
			LCD_clearScreen();
			LCD_displayString("Please Enter Five Numbs!");
			_delay_ms(500);
			LCD_clearScreen();
			LCD_displayString("Please Enter New");
			LCD_moveCursor(1, 0);
			LCD_displayString("Password:");
			i = 0;
		}
	}
}

void ask_pass(uint8 *key) {
	uint8 i = 0;
	LCD_clearScreen();
	LCD_displayString("Please Enter");
	LCD_moveCursor(1, 0);
	LCD_displayString("Password:");
	for (i = 0; i < 5; i++) {
		key[i] = KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(300);
	}
	key[5] = '#';
	key[6] = '\0';
	UART_sendString(key);
}

void old_pass(uint8 *key) {
	uint8 i = 0;
	LCD_clearScreen();
	LCD_displayString("Please Enter Old ");
	LCD_moveCursor(1, 0);
	LCD_displayString("Password :");
	_delay_ms(500);
	for (i = 0; i < 5; i++) {
		key[i] = KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(300);
	}
	key[5] = '#';
	key[6] = '\0';
	UART_sendString(key);
}

int main(void)
{
	/*******************************************************************************
	 *                           Initialization Section                            *
	 *******************************************************************************/
	uint8 check[6], key[7], option_key = 0, incorrect_pass = 0, i = 0;

	SREG |= (1 << 7);

	// Set configuration for Timer0
	Timer0_ConfigType timer_config = { normal, 0, 0, clk_256};

	// Initialize the UART driver with Baud-rate = 9600 bits/sec
	UART_init(9600);

	// Initialize LCD
	LCD_init();

	// Set Timer0 Call back function
	Set_Callback(Timer0_Tick);

	/*******************************************************************************
	 *                             Super Loop Section                              *
	 *******************************************************************************/
	// check if enter the password entered by user twice are right
//	while (match_check)
//	{
//		display_enter_pass();
//		get_pass(key);
//		display_renter_same();
//		get_pass(check);
//
//		match_check = pass_match_check(key, check);
//	}
//	match_check = 1;

	while (1) {
		first_run(key);
		first_run(check);
		for (i = 0; i < 5; i++) {
			if (key[i] == check[i]) {

			} else {
				break;
			}
		}
		if (i == 5) {
			break;
		} else {
			i = 0;
		}
	}

	key[5] = '#'; // termination Character to UART message
	key[6] = '\0';

	// Send the password to Control ECU to Save it in EEPROM
	UART_sendString(key);
	while (1) {
		main_options();
		option_key = KEYPAD_getPressedKey();
		_delay_ms(300);

		if (option_key == '+') {
			ask_pass(key);
//			// if user want to open the door
//			display_enter_pass();
//			get_pass(key);

//			// Send the password to Control ECU to compare it with pass in EEPROM
//			UART_sendString(key);
//			_delay_ms(10);

			// Send to Control_ECU that the user wants to open door
			UART_sendByte(option_key);

			// if the Password is the same compared to EEPROM, open the door
			option_key = UART_recieveByte();

			if (option_key == READY) {
				door_unlock();

				// wait 15s till door opens
				Timer0_init(&timer_config);
				while (Tick < 457);
				// disable timer0 & reset tick
				Timer0_Deinit();
				Tick = 0;

				//LCD_clearScreen();

				door_lock();

				Timer0_init(&timer_config);
				while (Tick < 457);
				// disable timer0 & reset tick
				Timer0_Deinit();
				Tick = 0;
			}
			// if the Password is not right print error message and count number of wrong inputs
			else {
				LCD_clearScreen();
				LCD_displayString("Wrong Pass!");
				incorrect_pass++;

				// if the user enter the password wrong three times Stop the System for one minute
				if (incorrect_pass == 3) {
					LCD_clearScreen();
					LCD_displayString("Multi Wrong!!");
					LCD_moveCursor(1, 0);
					LCD_displayString("Wait 1 minute...");

					Timer0_init(&timer_config);
					while (Tick < 1828);
					Timer0_Deinit();
					Tick = 0;
					incorrect_pass = 0;

				} else {
					_delay_ms(500);
				}
				LCD_clearScreen();
				LCD_displayString("Door is closed");
				_delay_ms(500);
			}
		}
		// if the user want to change password
		else if (option_key == '-') {
			old_pass(key);
//			display_enter_pass();
//			get_pass(key);
//
//			// Send the password to Control ECU to compare it with pass in EEPROM
//			UART_sendString(key);
//			_delay_ms(10);

			// Send to C_ECU that the user want to change password
			UART_sendByte(option_key);

			option_key = UART_recieveByte();

			// if the password is right get the new password
			if (option_key == READY) {

				// check if enter the password entered by user twice are right
				while (1) {
						first_run(key);
						first_run(check);
						for (i = 0; i < 5; i++) {
							if (key[i] == check[i]) {

							} else {
								break;
							}
						}
					if (i == 5) {
						key[5] = '#'; // termination Character to UART message
						key[6] = '\0';

						// send to ecu to save it at eeprom
						UART_sendString(key);
						break;
						} else {
							i = 0;
						}
					}

				key[5] = '#'; // termination Character to UART message
				key[6] = '\0';

				// send to ecu to save it at eeprom
				UART_sendString(key);
			}
		}
		/*if the Password is not right Print in LCD Wrong*/
		else {
			LCD_clearScreen();
			LCD_displayString("Wrong Pass!");
			incorrect_pass++;

			// if the user enter the password wrong three times Stop the System for one minute
			if (incorrect_pass >= 3) {
				LCD_clearScreen();
				LCD_displayString("Multi Wrong!!");
				LCD_moveCursor(1, 0);
				LCD_displayString("Wait 1 minute...");

				Timer0_init(&timer_config);
				while (Tick < 1828);
				Timer0_Deinit();
				Tick = 0;
				incorrect_pass = 0;
			} else {
				_delay_ms(500);
			}
		}
	}
}

void Timer0_Tick(void){
	Tick++;
}





