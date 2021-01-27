/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/gc.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "py/runtime.h"

#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/SleepMemory.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/alarm/touch/TouchAlarm.h"

#include "shared-bindings/microcontroller/__init__.h"

#include "supervisor/port.h"
#include "supervisor/shared/workflow.h"
#include "supervisor/board.h"

//#include "components/driver/include/driver/uart.h"

#include "nrfx/hal/nrf_power.h"
#include "nrfx/drivers/include/nrfx_power.h"
//#include "nrfx/mdk/nrf52_bitfields.h"
#include "nrf_nvic.h"
// Singleton instance of SleepMemory.
const alarm_sleep_memory_obj_t alarm_sleep_memory_obj = {
    .base = {
        .type = &alarm_sleep_memory_type,
    },
};


// modified
void alarm_reset(void) {
    alarm_sleep_memory_reset();
    alarm_pin_pinalarm_reset();
    alarm_time_timealarm_reset();
    //alarm_touch_touchalarm_reset();
}

// modified
STATIC uint32_t _get_wakeup_cause(void) {
    return nrf_power_resetreas_get(NRF_POWER);
}

// modified
bool alarm_woken_from_sleep(void) {
    return _get_wakeup_cause() & 0xF00;
}

// modified
STATIC mp_obj_t _get_wake_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    uint32_t cause = _get_wakeup_cause();

    if(cause & POWER_RESETREAS_DOG_Msk) {
        return alarm_time_timealarm_get_wakeup_alarm(n_alarms, alarms);
    } else if( cause & POWER_RESETREAS_OFF_Msk) {
        return alarm_pin_pinalarm_get_wakeup_alarm(n_alarms, alarms);
    } else if(0) {
        return alarm_touch_touchalarm_get_wakeup_alarm(n_alarms, alarms);
    }

    return mp_obj_new_int(cause);
}

// pass
mp_obj_t common_hal_alarm_get_wake_alarm(void) {
    return _get_wake_alarm(0, NULL);
}

// pass
// Set up light sleep or deep sleep alarms.
STATIC void _setup_sleep_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    alarm_pin_pinalarm_set_alarms(deep_sleep, n_alarms, alarms);
    alarm_time_timealarm_set_alarms(deep_sleep, n_alarms, alarms);
    //alarm_touch_touchalarm_set_alarm(deep_sleep, n_alarms, alarms);
}

// pass
#if(0)
STATIC void _idle_until_alarm(void) {
    // Poll for alarms.
    while (!mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
        // Allow ctrl-C interrupt.
        if (alarm_woken_from_sleep()) {
            alarm_save_wake_alarm();
            return;
        }
        port_idle_until_interrupt();
    }
}
#endif


mp_obj_t common_hal_alarm_light_sleep_until_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    _setup_sleep_alarms(false, n_alarms, alarms);
    NRF_POWER->RESETREAS = 0;
    // We cannot esp_light_sleep_start() here because it shuts down all non-RTC peripherals.
    //_idle_until_alarm();
    __WFE();
    
    if (mp_hal_is_interrupted()) {
        __SEV();
        return mp_const_none; // Shouldn't be given to python code because exception handling should kick in.
    }

    mp_obj_t wake_alarm = _get_wake_alarm(n_alarms, alarms);
    alarm_reset();
    __SEV();
    return wake_alarm;
}

void common_hal_alarm_set_deep_sleep_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    _setup_sleep_alarms(true, n_alarms, alarms);
}

//modified
void alarm_enter_deep_sleep(void) {
    alarm_pin_pinalarm_prepare_for_deep_sleep();
    board_before_deep_sleep();
    //nrf_power_system_off(NRF_POWER);
    sd_nvic_EnableIRQ(NRFX_GPIOTE_CONFIG_IRQ_PRIORITY);
    NRFX_IRQ_ENABLE(NRFX_GPIOTE_CONFIG_IRQ_PRIORITY);

    NRF_POWER->SYSTEMOFF = 1;
    while(1) { asm("NOP"); }
}

void common_hal_alarm_gc_collect(void) {
    gc_collect_ptr(alarm_get_wake_alarm());
}
