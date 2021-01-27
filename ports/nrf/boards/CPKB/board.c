/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/board.h"
#include "nrfx/hal/nrf_gpio.h"
#include "common-hal/microcontroller/Pin.h"

void board_init(void) {
  mcu_pin_vars_init();
  //reset_all_pins();
}

bool board_requests_safe_mode(void) {
  return false;
}

void reset_board(void) {

}

void board_deinit(void) {

}

#define ROW_COUNT 4
#define ROW_PINS {NRF_GPIO_PIN_MAP(1,0), NRF_GPIO_PIN_MAP(0,24), NRF_GPIO_PIN_MAP(0,22), NRF_GPIO_PIN_MAP(0,20)}
void board_before_deep_sleep(void) {
  uint32_t row_pins[ROW_COUNT] = ROW_PINS;
  for(uint8_t i=0;i<ROW_COUNT;i++) {
    nrf_gpio_cfg_output(row_pins[i]);
    nrf_gpio_pin_clear(row_pins[i]);
  }
}