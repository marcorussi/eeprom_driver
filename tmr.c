/*
 * The MIT License (MIT)
 *
 * Copyright (c) [2015] [Marco Russi]
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

/*
 * This file tmr.c represents the source file of the timer component.
 *
 * Author : Marco Russi
 *
 * Evolution of the file:
 * 13/09/2015 - File created - Marco Russi
 *
*/


/* --------------- Inclusion files ------------------ */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/f4/nvic.h>
#include <libopencm3/stm32/gpio.h>

#include "tmr.h"
#include "rtos.h"




/* --------------- Definitions ------------------ */




/* ------------ Exported functions declaration ----------------- */

/* Function to init a timer */
void timer_setup(void)
{
	/* Enable TIM2 clock. */
	rcc_periph_clock_enable(RCC_TIM2);

	/* Enable TIM2 interrupt. */
	nvic_enable_irq(NVIC_TIM2_IRQ);

	/* Reset TIM2 peripheral. */
	timer_reset(TIM2);

	/* Timer global mode:
	 * - No divider
	 * - Alignment edge
	 * - Direction up
	 */
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
					TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	/* Reset prescaler value.
	 * Running the clock at 5kHz.
	 */
	/*
	 * On STM32F4 the timers are not running directly from pure APB1 or
	 * APB2 clock busses.  The APB1 and APB2 clocks used for timers might
	 * be the double of the APB1 and APB2 clocks.  This depends on the
	 * setting in DCKCFGR register. By default the behaviour is the
	 * following: If the Prescaler APBx is greater than 1 the derived timer
	 * APBx clocks will be double of the original APBx frequencies. Only if
	 * the APBx prescaler is set to 1 the derived timer APBx will equal the
	 * original APBx frequencies.
	 *
	 * In our case here the APB1 is devided by 4 system frequency and APB2
	 * divided by 2. This means APB1 timer will be 2 x APB1 and APB2 will
	 * be 2 x APB2. So when we try to calculate the prescaler value we have
	 * to use rcc_apb1_freqency * 2!!!
	 *
	 * For additional information see reference manual for the stm32f4
	 * familiy of chips. Page 204 and 213
	 */
	timer_set_prescaler(TIM2, ((rcc_apb1_frequency * 2) / 10000));

	/* Disable preload. */
	timer_disable_preload(TIM2);

	/* Continous mode. */
	timer_continuous_mode(TIM2);

	/* Period (36kHz). */
	timer_set_period(TIM2, 100);

	/* Counter enable. */
	timer_enable_counter(TIM2);

	/* Enable update interrupt. */
	timer_enable_irq(TIM2, TIM_DIER_UIE);

}


/* Function to stop a timer */
void timer_stop(void)
{
	/* counter disable */
	timer_disable_counter(TIM2);

	/* disable update interrupt */
	timer_disable_irq(TIM2, TIM_DIER_UIE);
}




/* -------------- Local function declaration ----------------- */

/* TIM2 interrupt service routine */
void tim2_isr(void)
{
	/* manage update interrupt */
	if (timer_get_flag(TIM2, TIM_SR_UIF)) {

		/* call TICK timer callback */
		rtos_tick_timer_callback();

		/* Clear compare interrupt flag. */
		timer_clear_flag(TIM2, TIM_SR_UIF);
	} else {
		/* do nothing. ATTENTION: it could be better to clear all interrupts flags */
	}
}




/* End of file */

