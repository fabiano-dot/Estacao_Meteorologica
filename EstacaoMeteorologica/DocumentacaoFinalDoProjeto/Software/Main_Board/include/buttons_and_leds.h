/**
 * @file    buttons_and_leds.h
 * @author  Antonio-Carlos-Ricardo
 * @brief   Configura o Driver dos LEDs e do botão
 * @version 0.1
 * @date 2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef BUTTONS_AND_LEDS_H
#define BUTTONS_AND_LEDS_H

/**
 * @brief Define "cores"
 * 
 */
typedef enum{
    LED_BLACK = 0,
    LED_RED,
    LED_GREEN,
    LED_BLUE,
    LED_YELLOW,
    LED_MAGENTA,
    LED_CYAN,
    LED_WHITE
}LedsColor;



#define LED_INIT_DELAY_MS   400 // Tempo que cada LED vai ficar aceso na inicialização

/**
 * @brief Configura o Driver dos LEDs e botão
 *        - A porta do botão é configurada como input e com um pullup
 *        - A porta dos LEDs serão configuradas como output e como zero
 *        - Obs.: durante a inicialização cada LED fica aceso por: LED_INIT_DELAY_MS
 * 
 */
void buttons_and_leds_init();

/**
 * @brief Seta quais LEDs devem estar acesos ou apagados para refletir a cor
 * 
 * @param color Cor que os LEDs devem refletir
 */
void buttons_and_leds_set_color(LedsColor color);

/**
 * @brief Lê o estado do botão
 * 
 * @return true Botão pressionado
 * @return false Botão solto
 */
bool buttons_and_leds_button_pressed();


#endif // BUTTONS_AND_LEDS_H