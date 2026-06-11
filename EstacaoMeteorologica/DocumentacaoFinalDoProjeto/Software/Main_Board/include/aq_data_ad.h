/**
 * @file    aq_data_ad.h
 * @author  your name (you@domain.com)
 * @brief   Driver do AD para leitura da MPU Temperature e VSys
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef AQ_DATA_AD_H
#define AQ_DATA_AD_H

#include "pico/stdlib.h"

/**
 * @brief Configura o AD para baixo consumo de energia
 *        e armazena o fator de correção do VSYS
 * 
 * @param vsys Indica se será utilizado VSys
 * @param temp Indica se será utilizado MPU_Temp
 * @param vsys_k_10000 Fator de correção do VSYS
 *        - 10000 ==> multiplicador = 1.0
 *        - 20000 ==> multiplicador = 2.0
 * 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
int aqdataad_init_power_on(bool vsys, bool temp, uint16_t vsys_k_10000);
/**
 * @brief Inicializa o AD para aquisição
 * 
 * @param vsys Indica se será utilizado VSys
 * @param temp Indica se será utilizado MPU_Temp
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
int aqdataad_init_aq(bool vsys, bool temp);
/**
 * @brief Faz a leitura da temperatura da MPU
 * 
 * @param value Retorna o valor corrigido de VSys
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro 
 */
int aqdataad_read_temp(int16_t *value);

/**
 * @brief Faz a leitura, e corrige, do VSys
 * 
 * @param value Retorna o valor da temperatura do MPU
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
int aqdataad_read_vsys(uint8_t *value);

/**
 * @brief Desativa:
 *        - Sensor de temperatura
 *        - AD
 *        - Clock do AD
 * 
 * @param vsys Indica se será utilizado VSys
 * @param temp Indica se será utilizado MPU_Temp 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro 
 */
int aqdataad_sleep(bool vsys, bool temp);


#endif // AQ_DATA_AD_H