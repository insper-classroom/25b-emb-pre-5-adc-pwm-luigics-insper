/**
 * Copyright (c)2020 Raspberry Pi (Trading)Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/adc.h"
 
const int PIN_LED_B = 4;
const int PIN_ADC = 28;
const float conversion_factor = 3.3f / (1 << 12);
volatile int flag_led = 0;
repeating_timer_t timer_led;

bool timer_led_callback(repeating_timer_t *rt){
    flag_led = 1;
    return true;
}

/**
 * 0..1.0V: Desligado
 * 1..2.0V: 150 ms
 * 2..3.3V: 400 ms
*/

int main(){
    stdio_init_all();

    gpio_init(PIN_LED_B);
    gpio_set_dir(PIN_LED_B, GPIO_OUT);
    gpio_put(PIN_LED_B, 0);

    adc_init();
    adc_gpio_init(PIN_ADC);
    adc_select_input(2);

    int current_delay_ms = 0;
    bool est_led = false;
    bool primeira_vez = true; 

    while(1){
        uint16_t adc_raw = adc_read();
        float voltage = adc_raw * conversion_factor;

        int new_delay_ms = 0;

        if(voltage > 1.0f && voltage <= 2.0f){
            new_delay_ms = 300;
        } else if(voltage > 2.0f){
            new_delay_ms = 500;
        }

        if((new_delay_ms != current_delay_ms) || primeira_vez == true){
            current_delay_ms = new_delay_ms;
            primeira_vez = false;
            cancel_repeating_timer(&timer_led);

            if(current_delay_ms > 0){
                add_repeating_timer_ms(current_delay_ms, timer_led_callback, NULL, &timer_led);
            } else {
                est_led = false;
                gpio_put(PIN_LED_B, est_led);
            }
        }

        if(flag_led){
            flag_led = 0;//flag
            est_led = !est_led;
            gpio_put(PIN_LED_B, est_led);
        }
    }
}