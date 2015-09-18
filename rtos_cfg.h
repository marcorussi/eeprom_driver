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
 * This file rtos_cfg.h represents the header file of the RTOS configuration component.
 *
 * Author : Marco Russi
 *
 * Evolution of the file:
 * 06/08/2015 - File created - Marco Russi
 *
*/


#ifndef _RTOS_CFG_INCLUDED_         /* switch to read the headerfile only */
#define _RTOS_CFG_INCLUDED_         /* one time. */


/*==============================================================================
    Exported Constants
==============================================================================*/
enum {
	RTOS_CFG_KE_FIRST_STATE,
	RTOS_CFG_KE_INIT_STATE = RTOS_CFG_KE_FIRST_STATE,
	RTOS_CFG_KE_NORMAL_STATE,
	RTOS_CFG_KE_SLEEP_STATE,
	RTOS_CFG_KE_STATE_MAX_NUM
};


/*==============================================================================
    Exported Types
==============================================================================*/
/* Pointer to callback function */
typedef void (*callback_ptr_t)(void);

/* Pointer to RTOS task */
typedef void (*task_ptr_t)(void);

/* RTOS state */
typedef task_ptr_t const rtos_state_t;


/*==============================================================================
    Exported Variables
==============================================================================*/
extern rtos_state_t *const rtos_cfg_states_array[RTOS_CFG_KE_STATE_MAX_NUM];




#endif


/* END OF FILE rtos_cfg.h */
