/**
 * @file    menu_conf.h
 * @author  Antonio-Carlos-Ricardo
 * @brief   Ativa o menu de configuração da estação via USB
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef MENU_CONF_H
#define MENU_CONF_H

#include "est_config.h"

/**
 * @brief Ativa o menu de configuração da estação via USB
 * 
 * @param est_config Variável com a configuração atual que será mostrada e editada.
 * @param valid_flash_data Informa se os dados lidos da flash são válidos ou não.
 */
void menu_conf(EstConfig * est_config, bool valid_flash_data);

#endif // MENU_CONF_H