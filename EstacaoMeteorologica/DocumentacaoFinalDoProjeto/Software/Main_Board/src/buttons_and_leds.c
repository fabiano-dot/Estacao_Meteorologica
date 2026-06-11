/**
 * @file    buttons_and_leds.c
 * @author  Antonio-Carlos-Ricardo
 * @brief   Configura o Driver dos LEDs e do botão
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "pico/stdlib.h"
#include "../include/buttons_and_leds.h"
#include "../include/code_config.h"


void buttons_and_leds_init(){
    // Inicializa GPIOs dos botões
    gpio_init(   GPIO_BUTTON);
    gpio_pull_up(GPIO_BUTTON);

    // Inicializa GPIOs dos LEDs
    gpio_init(   GPIO_LED_RED);
    gpio_init(   GPIO_LED_GREEN);
    gpio_init(   GPIO_LED_BLUE);
    gpio_put(    GPIO_LED_RED,   false);
    gpio_put(    GPIO_LED_GREEN, false);
    gpio_put(    GPIO_LED_BLUE,  false);
    gpio_set_dir(GPIO_LED_RED,   GPIO_OUT);
    gpio_set_dir(GPIO_LED_GREEN, GPIO_OUT);
    gpio_set_dir(GPIO_LED_BLUE,  GPIO_OUT);

    // Teste dos LEDs
    buttons_and_leds_set_color(LED_RED);
    sleep_ms(LED_INIT_DELAY_MS);
    buttons_and_leds_set_color(LED_GREEN);
    sleep_ms(LED_INIT_DELAY_MS);
    buttons_and_leds_set_color(LED_BLUE);
    sleep_ms(LED_INIT_DELAY_MS);
    buttons_and_leds_set_color(LED_BLACK);
}

void buttons_and_leds_set_color(LedsColor color){
    switch(color){
        case LED_RED    : gpio_put(GPIO_LED_RED,  true), gpio_put(GPIO_LED_GREEN, false), gpio_put(GPIO_LED_BLUE, false); break;
        case LED_GREEN  : gpio_put(GPIO_LED_RED, false), gpio_put(GPIO_LED_GREEN,  true), gpio_put(GPIO_LED_BLUE, false); break;
        case LED_BLUE   : gpio_put(GPIO_LED_RED, false), gpio_put(GPIO_LED_GREEN, false), gpio_put(GPIO_LED_BLUE,  true); break;
        case LED_YELLOW : gpio_put(GPIO_LED_RED,  true), gpio_put(GPIO_LED_GREEN,  true), gpio_put(GPIO_LED_BLUE, false); break;
        case LED_MAGENTA: gpio_put(GPIO_LED_RED,  true), gpio_put(GPIO_LED_GREEN, false), gpio_put(GPIO_LED_BLUE,  true); break;
        case LED_CYAN   : gpio_put(GPIO_LED_RED, false), gpio_put(GPIO_LED_GREEN,  true), gpio_put(GPIO_LED_BLUE,  true); break;
        case LED_WHITE  : gpio_put(GPIO_LED_RED,  true), gpio_put(GPIO_LED_GREEN,  true), gpio_put(GPIO_LED_BLUE,  true); break;
        default         : gpio_put(GPIO_LED_RED, false), gpio_put(GPIO_LED_GREEN, false), gpio_put(GPIO_LED_BLUE, false); break;
    }
}

bool buttons_and_leds_button_pressed(){
    return !gpio_get(GPIO_BUTTON);
}
