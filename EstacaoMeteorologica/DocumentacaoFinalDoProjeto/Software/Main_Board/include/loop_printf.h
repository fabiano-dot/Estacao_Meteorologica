/**
 * @file    loop_printf.h
 * @author  Antonio-Carlos-Ricardo
 * @brief   Rotina que "embrulha" o printf em loop_printf
 *          Funciona como o printf só que
 *          a função loop_printf é chamada no lugar do printf pelas rotinas que são executadas dentro do main-loop
 *          Foi criada pensando em futuras melhorias, para ser usada quando o USB está off ==> inativar o printf
 * 
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef LOOP_PRINTF_H
#define LOOP_PRINTF_H

#include <stdio.h>
#include "code_config.h"


#ifdef LOOP_PRINTF_ENABLED
  #define loop_printf(...) printf(__VA_ARGS__)
#else
  #define loop_printf(...) ((void)0)
#endif


#endif // LOOP_PRINTF_H


