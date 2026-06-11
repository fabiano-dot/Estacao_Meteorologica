 /**
 * @file    aq_data_lux.h
 * @author  Antonio-Carlos-Ricardo
 * @brief   Driver do sensor de luminosidade BH1750
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef AQ_DATA_LUX_H
#define AQ_DATA_LUX_H

#include "pico/stdlib.h"

/**
 * @brief Define a variável do luxímetro
 * 
 */
typedef struct{
    uint16_t lux_level_x4;

} AqDataLux_Value;

/**
 * @brief Configura o driver após o reset
 * 
 * @param lux_k_10000 Fator de correção a ser utilizado 
 *        - 10000 ==> multiplicador = 1.0
 *        - 20000 ==> multiplicador = 2.0
 * 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
int aqdatalux_init_power_on(uint16_t lux_k_10000);

/**
 * @brief Inicializa o sensor para aquisição:
 * - Configura os registrados para iniciar a aquisição
 * - marca o instante do start da aquisição para a rotina aqdatalux_read saber quando o dado está pronto
 * 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
int aqdatalux_init_aq();

/**
 * @brief Faz a leitura do sensor:
 *        - Aguarda o tempo necessário para a aquisição;
 *        - Faz a leitura;
 *        - Aplica o fator de correção.
 * 
 * @param value Ponteiro para a variável que receberá a leitura corrigida do luxímetro.
 * 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
int aqdatalux_read(AqDataLux_Value *value);

/**
 * @brief Faz o sensor entrar em sleep-mode
 *        Obs.: não é necessário fazer nada, pois o modo de aquisição adotado já faz isto.
 * 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
int aqdatalux_sleep();


#endif // AQ_DATA_LUX_H