/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 microDev
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

#include "shared-bindings/alarm/touch/TouchAlarm.h"
#include "shared-bindings/microcontroller/__init__.h"

static volatile bool woke_up = false;

void common_hal_alarm_touch_touchalarm_construct(alarm_touch_touchalarm_obj_t *self, const mcu_pin_obj_t *pin) {
    
}

mp_obj_t alarm_touch_touchalarm_get_wakeup_alarm(const size_t n_alarms, const mp_obj_t *alarms) {
    mp_raise_ValueError(translate("Touch Alarm is not implemented yet."));
}

// This is used to wake the main CircuitPython task.
void touch_interrupt(void) {
    woke_up = true;
}

void alarm_touch_touchalarm_set_alarm(const bool deep_sleep, const size_t n_alarms, const mp_obj_t *alarms) {
    mp_raise_ValueError(translate("Touch Alarm is not implemented yet."));
}

void alarm_touch_touchalarm_prepare_for_deep_sleep(void) {
    mp_raise_ValueError(translate("Touch Alarm is not implemented yet."));
}

bool alarm_touch_touchalarm_woke_us_up(void) {
    mp_raise_ValueError(translate("Touch Alarm is not implemented yet."));
    return woke_up;
}

void alarm_touch_touchalarm_reset(void) {
    woke_up = false;
    mp_raise_ValueError(translate("Touch Alarm is not implemented yet."));
}
