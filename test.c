/*
* The MIT License (MIT)
*
* Copyright (c) 2015 Marco Russi
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/


/* ---------------- Inclusions ----------------- */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "test.h"
/* RTOS module */
#include "rtos.h"
/* EEPROM driver */
#include "eeprom.h"




/* --------------- Local defines ------------- */

/* EEPROM test page start address*/
#define EEPROM_TEST_PAGE_START_ADD			(0x0004)
/* EEPROM test byte address*/
#define EEPROM_TEST_BYTE_ADD				(0x0101)




/* --------------- Local functions prototypes ------------- */

/* EEPROM test states */
enum
{
	EEP_TEST_START,
	EEP_TEST_WRITE_BYTE = EEP_TEST_START,
	EEP_TEST_READ_BYTE,
	EEP_TEST_WRITE_PAGE,
	EEP_TEST_READ_PAGE,
	EEP_TEST_END_SUCCESS,
	EEP_TEST_END_FAIL
};




/* --------------- Local functions prototypes ------------- */

/* Store EEPROM test current state */
uint16_t eeprom_test_state = EEP_TEST_START;




/* --------------- Exported functions ------------- */

/* EEPROM test init */
void test_init(void)
{
	/* Enable GPIOD clock */
	rcc_periph_clock_enable(RCC_GPIOD);

	/* Set GPIO12, GPIO13, GPIO14, GPIO15 (in GPIO port D) to Alternate Function */
	gpio_mode_setup(GPIOD,
					GPIO_MODE_OUTPUT,
					GPIO_PUPD_NONE,
					GPIO12 | GPIO13 | GPIO14 | GPIO15);

	gpio_clear(GPIOD, GPIO12);
	gpio_clear(GPIOD, GPIO13);
	gpio_clear(GPIOD, GPIO14);
	gpio_clear(GPIOD, GPIO15);
}




/* EEPROM test task */
void test_task(void)
{
	/* EEPROM test */
	uint8_t test_byte;
	uint8_t test_buffer[22] = {'T','H','I','S','_','I','S','_',
							'A','N','_','E','E','P','R','O','M',
							'_','T','E','S','T'};
	uint8_t test_buffer_back[22] = {0,0,0,0,0,0,0,0,0,0,0,0,
									0,0,0,0,0,0,0,0,0,0};

	/* manage next state */
	switch (eeprom_test_state)
	{
	case EEP_TEST_WRITE_BYTE:
	{
		/* write the test byte to EEPROM */
		if (true == eeprom_write_byte(EEPROM_TEST_BYTE_ADD, 0xAD)) {
			/* write success, go to read back */
			eeprom_test_state = EEP_TEST_READ_BYTE;
		} else {
			/* write fail, EEPROM test fail */
			eeprom_test_state = EEP_TEST_END_FAIL;
		}
		break;
	}
	case EEP_TEST_READ_BYTE:
	{
		/* read back the test byte from EEPROM */
		if (true == eeprom_read_byte(EEPROM_TEST_BYTE_ADD, &test_byte)) {
			/* read success, check data validity */
			if (test_byte == 0xAD) {
				/* data is valid: go on with page test */
				eeprom_test_state = EEP_TEST_WRITE_PAGE;
			} else {
				/* data is not valid: EEPROM fail */
				eeprom_test_state = EEP_TEST_END_FAIL;
			}
		} else {
			/* read fail, EEPROM test fail */
			eeprom_test_state = EEP_TEST_END_FAIL;
		}
		break;
	}
	case EEP_TEST_WRITE_PAGE:
	{
		/* read back the test byte from EEPROM */
		if (true == eeprom_write_page(EEPROM_TEST_PAGE_START_ADD, test_buffer, 22)) {
			/* write success, go on to read back */
			eeprom_test_state = EEP_TEST_READ_PAGE;
		} else {
			/* write fail, EEPROM test fail */
			eeprom_test_state = EEP_TEST_END_FAIL;
		}
		break;
	}
	case EEP_TEST_READ_PAGE:
	{
		/* read back the test byte from EEPROM */
		if (true == eeprom_read_page(EEPROM_TEST_PAGE_START_ADD, test_buffer_back, 22)) {
			/* read success */
			if (memcmp(test_buffer, test_buffer_back, 22) == 0) {
				/* data is valid, EEPROM is fine */
				eeprom_test_state = EEP_TEST_END_SUCCESS;
			} else {
				/* data is not valid: EEPROM fail */
				eeprom_test_state = EEP_TEST_END_FAIL;
			}
		} else {
			/* read fail, EEPROM test fail */
			eeprom_test_state = EEP_TEST_END_FAIL;
		}
		break;
	}
	case EEP_TEST_END_SUCCESS:
	{
		/* set green LED */
		gpio_set(GPIOD, GPIO12);
		break;
	}
	case EEP_TEST_END_FAIL:
	{
		/* set red LED */
		gpio_set(GPIOD, GPIO14);
		break;
	}
	default:
	{
		/* do nothing */
	}
	}
}




/* End of file */



