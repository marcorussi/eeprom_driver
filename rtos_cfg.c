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
 * This file rtos_cfg.c represents the source file of the RTOS configuration component.
 *
 * Author : Marco Russi
 *
 * Evolution of the file:
 * 06/08/2015 - File created - Marco Russi
 *
*/


/* ------------- Inclusions ---------------- */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "rtos_cfg.h"		/* component RTOS configuration header file */
#include "eeprom.h"			/* EEPROM module */
#include "test.h"			/* TEST module */




/* -------------- Local Variables ------------------ */

/* INIT state tasks */
static void (*init_state_ptr_array[])(void) = {
	&eeprom_init,
	&test_init,
	NULL
};


/* NORMAL state tasks */
static void (*normal_state_ptr_array[])(void) = {
	&test_task,
	NULL
};


/* SLEEP state tasks */
static void (*sleep_state_ptr_array[])(void) = {
	NULL
};




/* ------------ Exported Variables ----------------- */

/* RTOS states array: This order shall be the same of RTOS_CFG_ke_states enum */
rtos_state_t * const rtos_cfg_states_array[RTOS_CFG_KE_STATE_MAX_NUM] = {
	init_state_ptr_array,
	normal_state_ptr_array,
	sleep_state_ptr_array
};




/* End of file */
