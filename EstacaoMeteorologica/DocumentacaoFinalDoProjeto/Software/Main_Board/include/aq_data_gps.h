/**
 * @file    aq_data_gps.h
 * @author  Antonio-Carlos-Ricardo
 * @brief   Driver do módulo de GPS neo6mv
 * @version 0.1
 * @date    2026-02-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef AD_DATA_GPS_H
#define AD_DATA_GPS_H

#include "pico/stdlib.h"

/**
 * @brief Dados lidos do GPS
 * 
 */
typedef struct{
    int32_t latitude;   // Latitude  em graus  = latitude  / (2^23)
    int32_t longitude;  // Longitude em graus  = longitude / (2^23)
    int16_t altitude;   // Altitude  em metros = altitude * 0.1
} AqDataGps_Value;

/**
 * @brief Inicializa o GPS
 *        - Inicializa UART do GPS
 *        - Disabilita mensagens NMEAs desnecessárias
 * 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
int aqdatagps_init_power_on();

/**
 * @brief 
 * 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
int aqdatagps_init_aq();

/**
 * @brief Lê as coordenadas GPS
 * 
 * @param value 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
int aqdatagps_read(AqDataGps_Value *value);

/**
 * @brief Coloca o GPS em sleep-mode
 *        Obs.: Este modelo de GPS não possui sleep-mode
 * 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
int aqdatagps_sleep();


#endif // AD_DATA_GPS_H