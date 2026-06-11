/**
 * @file    wrap_watchdog.c
 * @author  Antonio-Carlos-Ricardo
 * @brief   Rotinas que embrulham (wrap) as rotinas originais do wathdog
 *          Permitindo assim serem facilmente habilitadas ou desabilitadas
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "hardware/watchdog.h"
#include "../include/code_config.h"
#include "../include/wrap_watchdog.h"


void wrap_watchdog_disable(){
    #ifdef WATCHDOG_ENABLED
        watchdog_disable();
    #endif
}

void wrap_watchdog_enable(){
    #ifdef WATCHDOG_ENABLED
        watchdog_enable(WATCHDOG_TIME_MS, 1);
        watchdog_update();
    #endif
}

void wrap_watchdog_update(){
    #ifdef WATCHDOG_ENABLED
        watchdog_update();
    #endif
}
