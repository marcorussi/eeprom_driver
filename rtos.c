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
 * This file rtos.c represents the source file of the RTOS component.
 *
 * Author : Marco Russi
 *
 * Evolution of the file:
 * 06/08/2015 - File created - Marco Russi
 *
*/

/*
TODO: modify callbacks implementation in the same way of tasks (use flag end manage the counter in the tick int only)
TODO: state switch function shall return a valid value. Do not check it in the execution task function
*/

/* ------------ Inclusions -------------- */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "tmr.h"            /* component timer header file */

#include "rtos_cfg.h"       /* component config header file */
#include "rtos.h"           /* component header file */




/* ----------- Local constants definitions -------------- */

/* Tasks call counter timeout value */
#define UL_TASK_COUNTER_TIMEOUT         ((uint32_t)((RTOS_UL_TASKS_PERIOD_MS * 1000) / RTOS_UL_TICK_PERIOD_US))

/* Max callback time value in ms */
#define U32_CALLBACK_MAX_VALUE_MS       ((uint32_t)10000)     /* 10 s */

/* First task index */
#define U8_FIRST_TASK_INDEX_VALUE       0




/* ------------- Local typedef definitions ------------- */

/* tick timer enum definition */
enum {
	KE_TICK_TIMER_NOT_ELAPSED,
	KE_TICK_TIMER_ELAPSED
};




/* ------------- Local variables declaration --------------- */

/* store actual RTOS state (from switch_state) */
static uint8_t rtos_actual_state;

/* store actual tick timer status */
static uint8_t tick_timer_status;

/* store counters value of tasks call */
static uint32_t task_counters;

/* store expired flag of all callbacks */
static bool callback_expired_array[RTOS_CB_ID_MAX_NUM] = {
	false,
	false,
	false,
	false,
	false
};

/* store enable flag of all callbacks */
static bool callback_enabled_array[RTOS_CB_ID_MAX_NUM] = {
	false,
	false,
	false,
	false,
	false
};

/* store counters value of all callbacks */
static uint32_t callback_counters_array[RTOS_CB_ID_MAX_NUM];

/* store timeout value of all callbacks */
static uint32_t callback_timeout_array[RTOS_CB_ID_MAX_NUM];

/* store callback function pointers */
static callback_ptr_t callback_functions_ptr_array[RTOS_CB_ID_MAX_NUM] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};




/* ------------- Local functions prototypes ------------- */

static uint8_t get_new_state_to_switch(uint8_t);




/* --------------- Exported functions ---------------- */

/* set and start callback */
void rtos_set_callback(uint8_t callback_id,
						uint8_t callback_type,
						uint32_t timer_period_ms,
						void *callback_function_ptr)
{
    /* It is important to update timeout variable first and then counter
     * variable because an eventual tick interrupt can decrement counter value
     * during this operation. Because the timeout value is updated only
     * in periodic mode, this temp variable is necessary */
	uint32_t temp_counter;
	if ((callback_id < RTOS_CB_ID_CHECK)
	&& (callback_type < RTOS_CB_TYPE_CHECK)
	&& (timer_period_ms <= U32_CALLBACK_MAX_VALUE_MS)
	&& (callback_function_ptr != NULL)) {
		/* calculate counter value */
		temp_counter = (uint32_t)((timer_period_ms * 1000) / RTOS_UL_TICK_PERIOD_US);
		/* if periodic callback request */
		if (RTOS_CB_TYPE_PERIODIC == callback_type) {
			/* store timeout counter value */
			callback_timeout_array[callback_id] = temp_counter;
		}
		/* store counter value */
		callback_counters_array[callback_id] = temp_counter;
		/* store callback function pointer */
		callback_functions_ptr_array[callback_id] = callback_function_ptr;
		/* callback enabled. do it as last operation */
		callback_enabled_array[callback_id] = true;
	} else {
		/* invalid parameters */
	}
}


/* stop callback */
void rtos_stop_callback(uint8_t callback_id)
{
	if (callback_id < RTOS_CB_ID_CHECK) {
		/* callback disabled. do it as first operation */
		callback_enabled_array[callback_id] = false;
		/* clear timeout counter value */
		callback_timeout_array[callback_id] = 0;
		/* clear callback counter value */
		callback_counters_array[callback_id] = 0;
		/* clear callback function pointer */
		callback_functions_ptr_array[callback_id] = NULL;
	} else {
		/* invalid parameters */
	}
}


/* Manage RTOS tick timer */
void rtos_tick_timer_callback(void)
{
	uint8_t callback_index;

	/* decrement callback counter for each enabled callback */
	for (callback_index = 0; callback_index < RTOS_CB_ID_CHECK; callback_index++) {
		/* manage enabled callbacks only */
		if (callback_enabled_array[callback_index] == true) {
			/* if counter is not yet expired */
			if (callback_counters_array[callback_index] > 0) {
				/* decrement counter */
				callback_counters_array[callback_index]--;
			} else {
				/* set the flag to call the related callback function */
				callback_expired_array[callback_index] = true;

				/* if timeout value is valid than re-arm the counter */
				if (callback_timeout_array[callback_index] > 0) {
					/* callback is still enabled: re-arm counter */
					callback_counters_array[callback_index] = callback_timeout_array[callback_index];
				} else {
					/* it was a single callback: the callback is disabled now */
					callback_enabled_array[callback_index] = false;
				}
			}
		} else {
			/* else do nothing */
		}
	}

	/* if tasks counter is elapsed set timeout flag */
	if (task_counters > 0) {
		/* decrement tasks call counter */
		task_counters--;
	} else {
		/* re-arm tasks call counter value */
		task_counters = UL_TASK_COUNTER_TIMEOUT;

		/* indicate time base over */
		tick_timer_status = KE_TICK_TIMER_ELAPSED;
	}
}


/* Stop RTOS operation */
void rtos_stop_operation(void)
{
	/* stop tick timer */
	timer_stop();
}


/* Start RTOS operation: select required state if valid and start RTOS timer */
void rtos_start_operation(uint8_t required_state)
{
	/* arm tasks call counter value */
	task_counters = UL_TASK_COUNTER_TIMEOUT;

	/* check required state validity */
	if ((uint8_t)required_state < RTOS_CFG_KE_STATE_MAX_NUM) {
		/* select the requested RTOS state */
		rtos_actual_state = (uint8_t)required_state;

		/* Start tick timer */
		timer_setup();
	} else {
		/* invalid required state: do nothing */
	}
}


/* If tick is elapsed then execute all scheduled tasks and select new required state */
void rtos_execute_task(void)
{
	uint8_t task_index;
	uint8_t rtos_required_state;
	uint8_t callback_index;

	/* manage callback functions */
	for (callback_index = 0; callback_index < RTOS_CB_ID_CHECK; callback_index++) {
		/* if callback counter is expired  */
		if (callback_expired_array[callback_index] == true) {
			/* call callback function if pointer is valid (so, single callbacks are called once) */
			if (callback_functions_ptr_array[callback_index] != NULL) {
				/* call callback function */
				(*callback_functions_ptr_array[callback_index])();

				/* after function call clear the function pointer if the callback is now disabled */
				if (callback_enabled_array[callback_index] == false) {
					/* clear function pointer: stop callback */
					callback_functions_ptr_array[callback_index] = NULL;
				}
			} else {
				/* else callback is disabled, do nothing */
			}

			/* clear expired flag */
			callback_expired_array[callback_index] = false;
		} else {
			/* else leave it to expire */
		}
	}

	/* check if RTOS time base is elapsed */
	if (KE_TICK_TIMER_ELAPSED == tick_timer_status) {
		/* set tick timer not elapsed */
		tick_timer_status = KE_TICK_TIMER_NOT_ELAPSED;

		/* execute all tasks in actual selected RTOS state */
		for (task_index = U8_FIRST_TASK_INDEX_VALUE;
			rtos_cfg_states_array[rtos_actual_state][task_index] != NULL;
			task_index++) {
			/* call actual selected task of actual RTOS state */
			(*rtos_cfg_states_array[rtos_actual_state][task_index])();
		}

		/* load new system state */
		rtos_required_state =
				get_new_state_to_switch(rtos_actual_state);

		/* new system state supported? */
		if (rtos_required_state < RTOS_CFG_KE_STATE_MAX_NUM) {
			/* enter new system state */
			rtos_actual_state = rtos_required_state;
		} else {
			/* remain in actual system state */
		}
	} else {
		/* do nothing */
	}
}




/* -------------- Local functions implementation ----------------- */

/* This function determines the next RTOS mode */
static uint8_t get_new_state_to_switch(uint8_t actual_state)
{
	uint8_t required_state_return;

	/* switch to system state */
	switch (actual_state) {
	/* RTOS_CFG_KE_INIT_STATE state? */
	case RTOS_CFG_KE_INIT_STATE:
	{
		/* enter RTOS_CFG_KE_NORMAL_STATE state */
		required_state_return = RTOS_CFG_KE_NORMAL_STATE;
		break;
	}
	/* RTOS_CFG_KE_NORMAL_STATE state? */
	case RTOS_CFG_KE_NORMAL_STATE:
	{
		/* remain in RTOS_CFG_KE_NORMAL_STATE state */
		required_state_return = RTOS_CFG_KE_NORMAL_STATE;
		break;
	}
	/* RTOS_CFG_KE_SLEEP_STATE state? */
	case RTOS_CFG_KE_SLEEP_STATE:
	{
		break;
	}
	/* default */
	default:
	{
		break;
	}
	}

	/* return new determined system state */
	return required_state_return;
}




/* End of file */
