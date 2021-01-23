/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include "py/runtime.h"

#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "nrfx/hal/nrf_power.h"
#include "peripherals/nrf/pins.h"
#include "nrfx/drivers/include/nrfx_gpiote.h"
#include "nrfx/hal/nrf_gpio.h"
#include "nrf_nvic.h"
#include "nrf/pins.h"

#include "common-hal/microcontroller/Pin.h"

void error_handler(void);
uint32_t alarm_pin_pinalarm_enables[2] = {  0, 0 };
uint32_t alarm_pin_pinalarm_sense[2] = { 0, 0 };
uint32_t alarm_pin_pinalarm_edge[2] = { 0, 0 };
uint32_t alarm_pin_pinalarm_pull[2] = { 0, 0 };

void common_hal_alarm_pin_pinalarm_construct(alarm_pin_pinalarm_obj_t *self, mcu_pin_obj_t *pin, bool value, bool edge, bool pull) {
    self->pin = pin;
    self->edge = edge;
    self->value = value;
    self->pull = pull;
}

mcu_pin_obj_t *common_hal_alarm_pin_pinalarm_get_pin(alarm_pin_pinalarm_obj_t *self) {
    return self->pin;
}

bool common_hal_alarm_pin_pinalarm_get_value(alarm_pin_pinalarm_obj_t *self) {
    return self->value;
}

bool common_hal_alarm_pin_pinalarm_get_edge(alarm_pin_pinalarm_obj_t *self) {
    return self->edge;
}

bool common_hal_alarm_pin_pinalarm_get_pull(alarm_pin_pinalarm_obj_t *self) {
    return self->pull;
}

mcu_pin_obj_t alarm_pin_pinalarm_cause;
nrf_gpiote_polarity_t alarm_pin_pinalarm_action;


void reset_interrupt_flag(void) {
    alarm_pin_pinalarm_cause.number = (1<<(5+1)) | 0;
}
// nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action
void gpio_interrupt(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    alarm_pin_pinalarm_cause.number = pin;
    alarm_pin_pinalarm_action = action;
}

// modified
bool alarm_pin_pinalarm_woke_us_up(void) {
    uint32_t reg = nrf_power_resetreas_get(NRF_POWER);
    return reg & (1<<4);
}

mp_obj_t alarm_pin_pinalarm_get_wakeup_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    // First, check to see if we match any given alarms.
    for (size_t i = 0; i < n_alarms; i++) {
        if (!MP_OBJ_IS_TYPE(alarms[i], &alarm_pin_pinalarm_type)) {
            continue;
        }
        alarm_pin_pinalarm_obj_t *alarm  = MP_OBJ_TO_PTR(alarms[i]);
        
        if (alarm_pin_pinalarm_cause.number == alarm->pin->number) {
            alarm_pin_pinalarm_reset();
            return alarms[i];
        }
    }
    
    alarm_pin_pinalarm_obj_t *alarm = m_new_obj(alarm_pin_pinalarm_obj_t);
    alarm->base.type = &alarm_pin_pinalarm_type;
    alarm->pin = NULL;
    return alarm;
}

// These must be static because we need to configure pulls later, right before
// deep sleep.

void alarm_pin_pinalarm_reset(void) {
    reset_interrupt_flag();
}

void alarm_pin_pinalarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    // Bitmask of wake up settings.
    if(! nrfx_gpiote_is_init() ) {
        nrfx_gpiote_init(GPIOTE_IRQn);
    }

    nrfx_gpiote_in_config_t in_config;
    for (size_t i = 0; i < n_alarms; i++) {
        // TODO: Check for ULP or touch alarms because they can't coexist with GPIO alarms.
        if (!MP_OBJ_IS_TYPE(alarms[i], &alarm_pin_pinalarm_type)) {
            continue;
        }
        alarm_pin_pinalarm_obj_t *alarm  = MP_OBJ_TO_PTR(alarms[i]);
        never_reset_pin_number(alarm->pin->number);
        alarm_pin_pinalarm_enables[nrf_pin_port(alarm->pin->number)] |= (1<<nrf_relative_pin_number(alarm->pin->number) );
        alarm_pin_pinalarm_pull[nrf_pin_port(alarm->pin->number)] |= ( (alarm->pull)<<nrf_relative_pin_number(alarm->pin->number) );
        alarm_pin_pinalarm_sense[nrf_pin_port(alarm->pin->number)] |= ( (alarm->value)<<nrf_relative_pin_number(alarm->pin->number) );
        alarm_pin_pinalarm_edge[nrf_pin_port(alarm->pin->number)] |= ( (alarm->edge)<<nrf_relative_pin_number(alarm->pin->number) );
        if(alarm->edge) {
            in_config.is_watcher = false;
            in_config.hi_accuracy = false;
            in_config.skip_gpio_setup = false;
            if (alarm->value) {
                in_config.sense = NRF_GPIOTE_POLARITY_LOTOHI;
            } else {
                in_config.sense = NRF_GPIOTE_POLARITY_HITOLO;
            }
            if (alarm->pull) {
                in_config.pull = NRF_GPIO_PIN_PULLUP;
            } else {
                in_config.pull = NRF_GPIO_PIN_NOPULL;
            }
            nrfx_gpiote_in_init(alarm->pin->number, &in_config, *gpio_interrupt);
            nrfx_gpiote_in_event_enable(alarm->pin->number, true);

        } else {
            // level alarm
            nrf_gpio_cfg_sense_input(alarm->pin->number, 
                    alarm->pull ? NRF_GPIO_PIN_PULLUP : NRF_GPIO_PIN_NOPULL,
                    alarm->value ? NRF_GPIO_PIN_SENSE_HIGH : NRF_GPIO_PIN_SENSE_LOW);
        }
    }
}


void alarm_pin_pinalarm_prepare_for_deep_sleep(void) {
    nrfx_gpiote_in_config_t in_config;
    in_config.is_watcher = false;
    in_config.hi_accuracy = false;
    in_config.skip_gpio_setup = false;

    for(uint8_t i=0;i<2;i++) {
        for(uint8_t j=0; j<32; j++) {
            if( alarm_pin_pinalarm_enables[i] & (1<<j) ) {
                if( alarm_pin_pinalarm_edge[i] & (1<<j) ) {
                    if ( alarm_pin_pinalarm_sense[i] & (1<<j) ) {
                        in_config.sense = NRF_GPIOTE_POLARITY_LOTOHI;
                    } else {
                        in_config.sense = NRF_GPIOTE_POLARITY_HITOLO;
                    }
                    if ( alarm_pin_pinalarm_pull[i] & (1<<j) ) {
                        in_config.pull = NRF_GPIO_PIN_PULLUP;
                    } else {
                        in_config.pull = NRF_GPIO_PIN_NOPULL;
                    }
                    nrfx_gpiote_in_init(NRF_GPIO_PIN_MAP(i,j), &in_config, *gpio_interrupt);
                    nrfx_gpiote_in_event_enable(NRF_GPIO_PIN_MAP(i,j), true);
                } else {
                    nrf_gpio_cfg_sense_input(NRF_GPIO_PIN_MAP(i,j),
                        ( alarm_pin_pinalarm_pull[i] & (1<<j) )  ? NRF_GPIO_PIN_PULLUP : NRF_GPIO_PIN_NOPULL,
                        ( alarm_pin_pinalarm_sense[i] & (1<<j) ) ? NRF_GPIO_PIN_SENSE_HIGH : NRF_GPIO_PIN_SENSE_LOW);
                }
            }
        }
    }
    NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Msk;
    uint32_t error = sd_nvic_EnableIRQ(GPIOTE_IRQn);
    if( error != NRF_SUCCESS ) {
        error_handler();
    }
}

void error_handler(void) {
    nrf_gpio_pin_dir_set(13, NRF_GPIO_PIN_DIR_OUTPUT);
    
    nrf_gpio_pin_set(13);
    while(1) {
    }
}
/*
void GPIOTE_IRQHandler(void) {
    if(NRF_GPIOTE->EVENTS_PORT){
        NRF_GPIOTE->EVENTS_PORT = 0;
    }
}
*/
