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
#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/f4/nvic.h>

#include "eeprom.h"


/* ---------------- Local Defines ----------------- */




/* ---------------- Local Macros ----------------- */




/* ----------- Local variables declaration ------------- */




/* ----------- Local functions prototypes ------------- */




/* ------------- Exported functions implementation --------------- */

/* Function to init EEPROM driver and I2C peripheral */
void eeprom_init(void)
{
	/* Enable GPIOB clock. */
	rcc_periph_clock_enable(RCC_GPIOB);
	/* set I2C1_SCL and I2C1_SDA, external pull-up resistors */
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO7);
	/* Open Drain, Speed 100 MHz */
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_100MHZ, GPIO6 | GPIO7);
	/* Alternate Function: I2C1 */
	gpio_set_af(GPIOB, GPIO_AF4,  GPIO6 | GPIO7);

	/* Enable I2C1 clock. */
	rcc_periph_clock_enable(RCC_I2C1);
	/* Enable I2C1 interrupt. */
	nvic_enable_irq(NVIC_I2C1_EV_IRQ);
	/* reset I2C1 */
	i2c_reset(I2C1);
	/* standard mode */
	i2c_set_standard_mode(I2C1);
	/* clock and bus frequencies */
	i2c_set_clock_frequency(I2C1, I2C_CR2_FREQ_2MHZ);
	i2c_set_ccr(I2C1, 20);
	/* enable error event interrupt only */
	i2c_enable_interrupt(I2C1, I2C_CR2_ITERREN);
	/* enable I2C */
	i2c_peripheral_enable(I2C1);
}


/* Function to write a byte at a specific address */
bool eeprom_write_byte(uint16_t address, uint8_t data)
{
	bool success = true;

	/* send START and wait for completion */
	i2c_send_start(I2C1);
	while ((I2C_SR1(I2C1) & I2C_SR1_SB) == 0);

	/* send device address, r/w request and wait for completion */
	i2c_send_7bit_address(I2C1, 0x51, I2C_WRITE);
	while ((I2C_SR1(I2C1) & I2C_SR1_ADDR) == 0);

	/* check SR2 and go on if OK */
	if ((I2C_SR2(I2C1) & I2C_SR2_MSL)		/* master mode */
	&&	(I2C_SR2(I2C1) & I2C_SR2_BUSY)) {	/* communication ongoing  */

		/* send memory address MSB */
		i2c_send_data(I2C1, ((uint8_t)(address >> 8)));
		while ((I2C_SR1(I2C1) & I2C_SR1_TxE) == 0);

		/* send memory address LSB */
		i2c_send_data(I2C1, ((uint8_t)address));
		while ((I2C_SR1(I2C1) & I2C_SR1_TxE) == 0);

		/* send data byte */
		i2c_send_data(I2C1, data);
		while ((I2C_SR1(I2C1) & I2C_SR1_TxE) == 0);

		/* send stop */
		i2c_send_stop(I2C1);

		/* ATTENTION: consider to wait for a while */
	} else {
		/* error */
		success = false;
	}

	return success;
}


/* Function to write a page starting from a specific address */
bool eeprom_write_page(uint16_t address, uint8_t *data_ptr, uint16_t data_length)
{
	bool success = true;

	/* send START and wait for completion */
	i2c_send_start(I2C1);
	while ((I2C_SR1(I2C1) & I2C_SR1_SB) == 0);

	/* send device address, r/w request and wait for completion */
	i2c_send_7bit_address(I2C1, 0x51, I2C_WRITE);
	while ((I2C_SR1(I2C1) & I2C_SR1_ADDR) == 0);

	/* check SR2 and go on if OK */
	if ((I2C_SR2(I2C1) & I2C_SR2_MSL)		/* master mode */
	&&	(I2C_SR2(I2C1) & I2C_SR2_BUSY)) {	/* communication ongoing  */

		/* send memory address MSB */
		i2c_send_data(I2C1, ((uint8_t)(address >> 8)));
		while ((I2C_SR1(I2C1) & I2C_SR1_TxE) == 0);

		/* send memory address LSB */
		i2c_send_data(I2C1, ((uint8_t)address));
		while ((I2C_SR1(I2C1) & I2C_SR1_TxE) == 0);

		/* write all bytes */
		while (data_length > 0) {
			/* send next data byte */
			i2c_send_data(I2C1, *data_ptr);
			/* increment data buffer pointer and
			 * decrement data buffer length */
			data_ptr++;
			data_length--;
			while ((I2C_SR1(I2C1) & I2C_SR1_TxE) == 0);
		}
		/* send stop */
		i2c_send_stop(I2C1);

		/* ATTENTION: consider to wait for a while */
	} else {
		/* error */
		success = false;
	}

	return success;
}


/* Function to write a byte at a specific address */
bool eeprom_read_byte(uint16_t address, uint8_t *byte_ptr)
{
	bool success = true;

	/* send START and wait for completion */
	i2c_send_start(I2C1);
	while ((I2C_SR1(I2C1) & I2C_SR1_SB) == 0);

	/* send device address, write request and wait for completion */
	i2c_send_7bit_address(I2C1, 0x51, I2C_WRITE);
	while ((I2C_SR1(I2C1) & I2C_SR1_ADDR) == 0);

	/* check SR2 and go on if OK */
	if ((I2C_SR2(I2C1) & I2C_SR2_MSL)		/* master mode */
	&&	(I2C_SR2(I2C1) & I2C_SR2_BUSY)) {	/* communication ongoing  */

		/* send memory address MSB */
		i2c_send_data(I2C1, ((uint8_t)(address >> 8)));
		while ((I2C_SR1(I2C1) & I2C_SR1_TxE) == 0);

		/* send memory address LSB */
		i2c_send_data(I2C1, ((uint8_t)address));
		while ((I2C_SR1(I2C1) & I2C_SR1_TxE) == 0);

		/* send START and wait for completion */
		i2c_send_start(I2C1);
		while ((I2C_SR1(I2C1) & I2C_SR1_SB) == 0);

		/* send device address, read request and wait for completion */
		i2c_send_7bit_address(I2C1, 0x51, I2C_READ);
		while ((I2C_SR1(I2C1) & I2C_SR1_ADDR) == 0);

		/* if communication ongoing  */
		if (I2C_SR2(I2C1) & I2C_SR2_BUSY) {
			/* read received byte */
			while ((I2C_SR1(I2C1) & I2C_SR1_RxNE) == 0);
			*byte_ptr = i2c_get_data(I2C1);
			/* send stop */
			i2c_send_stop(I2C1);
		} else {
			/* error */
			success = false;
		}
	} else {
		/* error */
		success = false;
	}

	return success;
}


/* Function to read a page starting from a specific address */
bool eeprom_read_page(uint16_t address, uint8_t *byte_ptr, uint16_t data_length)
{
	bool success = true;

	/* send START and wait for completion */
	i2c_send_start(I2C1);
	while ((I2C_SR1(I2C1) & I2C_SR1_SB) == 0);

	/* send device address, write request and wait for completion */
	i2c_send_7bit_address(I2C1, 0x51, I2C_WRITE);
	while ((I2C_SR1(I2C1) & I2C_SR1_ADDR) == 0);

	/* check SR2 and go on if OK */
	if ((I2C_SR2(I2C1) & I2C_SR2_MSL)		/* master mode */
	&&	(I2C_SR2(I2C1) & I2C_SR2_BUSY)) {	/* communication ongoing  */

		/* send memory address MSB */
		i2c_send_data(I2C1, ((uint8_t)(address >> 8)));
		while ((I2C_SR1(I2C1) & I2C_SR1_TxE) == 0);

		/* send memory address LSB */
		i2c_send_data(I2C1, ((uint8_t)address));
		while ((I2C_SR1(I2C1) & I2C_SR1_TxE) == 0);

		/* send START and wait for completion */
		i2c_send_start(I2C1);
		while ((I2C_SR1(I2C1) & I2C_SR1_SB) == 0);

		/* send device address, read request and wait for completion */
		i2c_send_7bit_address(I2C1, 0x51, I2C_READ);
		while ((I2C_SR1(I2C1) & I2C_SR1_ADDR) == 0);

		/* if communication ongoing  */
		if (I2C_SR2(I2C1) & I2C_SR2_BUSY) {
			/* enable ACK */
			i2c_enable_ack(I2C1);
			/* read all bytes */
			while (data_length > 0) {
				/* read received byte */
				while ((I2C_SR1(I2C1) & I2C_SR1_RxNE) == 0);
				*byte_ptr = i2c_get_data(I2C1);
				/* increment data buffer pointer and
				 * decrement data buffer length */
				byte_ptr++;
				data_length--;
				/* if last byte is remaining */
				if (data_length == 1) {
					/* disable ACK */
					i2c_disable_ack(I2C1);
				}
			}
			/* send stop */
			i2c_send_stop(I2C1);
		} else {
			/* error */
			success = false;
		}
	} else {
		/* error */
		success = false;
	}

	return success;
}




/* ------------ Local functions implementation -------------- */

void i2c1_ev_isr(void)
{
	gpio_set(GPIOD, GPIO14);
}




/* End of file */
