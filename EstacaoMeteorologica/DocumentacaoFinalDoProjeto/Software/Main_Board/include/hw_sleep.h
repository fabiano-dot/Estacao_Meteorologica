/**
 * @file    hw_sleep.h
 * @author  Antonio-Carlos-Ricardo
 * @brief   Administra o sleep-mode do rp2040 para reduzir o consumo
 * @version 0.1
 * @date    2026-02-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef HW_SLEEP_H
#define HW_SLEEP_H

#include "pico/stdlib.h"
#include "est_config.h"

/**
 * @brief 
 * 
 * @param usb_mode Informa qual é o USB Mode
 * @param ad_on Informa que o AD
 * @param sleep_minutes 
 */
void hw_sleep_init(UsbMode usb_mode, bool ad_on, uint16_t sleep_minutes);

/**
 * @brief Faz o RP2040 entrar em sleep-mode por um período pré configurado de tempo
 * 
 */
void hw_sleep();


#endif // HW_SLEEP_H