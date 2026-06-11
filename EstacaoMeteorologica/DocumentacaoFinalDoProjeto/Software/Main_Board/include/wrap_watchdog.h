/**
 * @file    wrap_watchdog.h
 * @author  Antonio-Carlos-Ricardo
 * @brief   Rotinas que embrulham (wrap) as rotinas originais do wathdog
 *          Permitindo assim serem facilmente habilitadas ou desabilitadas
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef WRAP_WATCHDOG_H
#define WRAP_WATCHDOG_H

/**
 * @brief Desabilita o Wathdog
 * 
 */
void wrap_watchdog_disable();

/**
 * @brief Habilita o Watchdog com timeout de: WATCHDOG_TIME_MS
 * 
 */
void wrap_watchdog_enable();

/**
 * @brief Atualiza o relógio do Watchdog para ele não disparar
 * 
 */
void wrap_watchdog_update();


#endif // WRAP_WATCHDOG_H