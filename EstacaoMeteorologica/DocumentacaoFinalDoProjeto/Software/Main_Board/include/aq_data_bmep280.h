/**
 * @file    aq_data_bmep280.h
 * @author  Antonio-Carlos-Ricardo
 * @brief   Driver do sensor BME280 ou BMP280
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef AD_DATA_BMEP280_H
#define AD_DATA_BMEP280_H

#include "pico/stdlib.h"


#define BMEP280_ADDRESS     0x76    // Alternativa 0X77

/**
 * @brief Dispositivos compatíveis
 * 
 */
typedef enum{
    AQDATABMEP280_DEV_ERROR = 0,
    AQDATABMEP280_DEV_BMP280,
    AQDATABMEP280_DEV_BME280
} AqDataBmep280_Device;

/**
 * @brief Valores inteiros que serão retornados
 * 
 */
typedef struct{
    uint8_t  humidity;      // Umidade     em %       =        humidity * 0.5
    int16_t  temp;          // Temperatura em Celsius =            temp * 0.01
    uint16_t press;         // Pressão     em hPa     = (60000 - press) * 0.02
} AqDataBmep280_Value_I;


/**
 * @brief Identifica qual dispositivo é (BME280 ou BMP280) e lê os valores de calibração
 * 
 * @return  int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro 
 */
int aqdatabmep280_init_power_on();

/**
 * @brief Configura o modo de leitura e dispara a aquisição
 * 
 * @return  int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro 
 */
int aqdatabmep280_init_aq();

/**
 * @brief Lê a aquisição:
 *        - Aguarda a aquisição ser concluida
 *        - Lê a aquisição
 *        - Aplica os fatores de calibração
 * 
 * @param value 
 * @return  int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
int aqdatabmep280_read_i(AqDataBmep280_Value_I *value);

/**
 * @brief Faz o sensor entrar em sleep-mode
 *        Obs.: não é necessário fazer nada, pois o modo de aquisição adotado (Forced Mode) já faz isto.
 *
 * @return  int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
int aqdatabmep280_sleep();



/**
 * @brief Rotina para teste do sensor
 *        Uso no desenvolvimento
 *        Compara formulas de compensação com inteiro vs double
 * 
 */
void aqdatabmep280_teste();

#endif // AD_DATA_BMEP280_H