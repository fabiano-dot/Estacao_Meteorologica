/**
 * @file    aq_data.h
 * @author  Antonio-Carlos-Ricardo
 * @brief   Chama todos os drivers dos sensores
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef AQ_DATA_H
#define AQ_DATA_H

#include "pico/stdlib.h"
#include "aq_data_ad.h"
//#include "aq_data_bat.h"
#include "aq_data_bmep280.h"
#include "aq_data_gps.h"
#include "aq_data_lux.h"
#include "est_config.h"

/**
 * @brief Define o flag (bit) utilizado por cada sensor.
 * 
 */
typedef enum{
//    AQ_ITEM_BAT_VALUE = 0x01,
    AQ_ITEM_BME280    = 0x02,
    AQ_ITEM_GPS       = 0x04,
    AQ_ITEM_LUX       = 0x08,
    AQ_ITEM_UV        = 0x10,
    AQ_ITEM_WIND      = 0x20,
    AQ_ITEM_VSYS      = 0x40,
    AQ_ITEM_CPU_TEMP  = 0x80
} AqItem;

/**
 * @brief Define uma estrutura de variável que contempla todos os sensores
 * 
 */
typedef struct{
    ActiveSensors         active_sensors;
//    AqDataBat_Value       battery;
    AqDataBmep280_Value_I bmep280;
    AqDataGps_Value       gps;
    AqDataLux_Value       lux;
    uint8_t               vsys;
    int16_t               cpu_temp_deci;
    // add futuros
} AqData;

/**
 * @brief Configura todos os drivers dos sensores habilitados após o reset
 *        - Também configura o I2C dos sensores
 * 
 * @param value Ponteiro da variável que contêm:
 *        - os sensores ativos
 *        - o resultado das leituras dos sensores
 * @param lux_k_10000 Fator de correção a ser utilizado pelo  BH1750
 *        - 10000 ==> multiplicador = 1.0
 *        - 20000 ==> multiplicador = 2.0
 * @param vsys_k_10000 Fator de correção a ser utilizado VSys
 *        - 10000 ==> multiplicador = 1.0
 *        - 20000 ==> multiplicador = 2.0
 * 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro 
 */
int aqdata_init_power_on(AqData *value, uint16_t lux_k_10000, uint16_t vsys_k_10000);

/**
 * @brief Inicializa todos os sensores habilitados para aquisição
 * 
 * @param value Ponteiro da variável que contêm:
 *        - os sensores ativos
 *        - o resultado das leituras dos sensores
 * 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
int aqdata_init_aq(      AqData *value);

/**
 * @brief Faz a leitura de todos os sensores habilitados:
 * 
 * @param value Ponteiro da variável que contêm:
 *        - os sensores ativos
 *        - o resultado das leituras dos sensores
 * 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro 
 */
int aqdata_read(         AqData *value);

/**
 * @brief Faz os sensores habilitados entrarem em sleep-mode
 * 
 * @param value Ponteiro da variável que contêm:
 *        - os sensores ativos
 *        - o resultado das leituras dos sensores
 *           
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
int aqdata_sleep(        AqData *value);

#endif // AQ_DATA_H