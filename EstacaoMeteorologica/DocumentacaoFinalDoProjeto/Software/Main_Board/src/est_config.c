/**
 * @file    est_config.c
 * @author  Antonio-Carlos-Ricardo
 * @brief   Define a estrutura de configuração e as manipula
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <string.h>
#include "../include/code_config.h"
#include "../include/est_config.h"
#include "../include/storage.h"

// Vaslores de início para os cálculos de Sun e Xor
#define CONFIG_DATA_SUN_START_VALUE 0x12345678
#define CONFIG_DATA_XOR_START_VALUE 0x87

//static const uint8_t dev_add[4]    = {0x12, 0x34, 0x56, 0x78};
//static const uint8_t app_s_key[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};     // AppSKey	16 bytes	Chave da aplicação
//static const uint8_t nwk_s_key[16] = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};     // NwkSKey	16 bytes	Chave de rede

// Vaslores já definidos no TTN
static const uint8_t dev_add[4]    = {0x26, 0x0C, 0x29, 0x3F};
static const uint8_t app_s_key[16] = {0x6D, 0xEF, 0x7D, 0x26, 0x8C, 0x81, 0x0B, 0xB9, 0xB1, 0x35, 0xF6, 0x5E, 0x0D, 0x67, 0x3D, 0x33};
static const uint8_t nwk_s_key[16] = {0x48, 0x85, 0xEA, 0x88, 0xC4, 0xCA, 0xB6, 0xF8, 0x24, 0xDE, 0xFD, 0x47, 0xC2, 0x0A, 0x1D, 0x63};


static const uint8_t dev_eui[8]    = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};                                                       // DevEUI	              8 bytes	Identificador único do dispositivo
static const uint8_t app_eui[8]    = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};                                                       // AppEUI (ou JoinEUI)  8 bytes	Identificador da aplicação
static const uint8_t app_key[16]   = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};       // AppKey	             16 bytes	Chave secreta (AES-128)


/**
 * @brief Calcula e/ou verifica os HASHs dos dados de configuração
 * 
 * @param set 
 *        - true calcula e salva o HASH
 *        - false calcula e verifica o HASH
 * @return uint8_t, Código de erro:
 *         - 0: HASH OK
 *         - diferente de 0: HASH ERRO
 */
static uint8_t config_data_error_check(EstConfig * est_config, bool set){
    int       size = sizeof(EstConfig) - 8;
    uint8_t * p    = (uint8_t *)est_config;

    uint32_t sum = CONFIG_DATA_SUN_START_VALUE;
    uint8_t  xor = CONFIG_DATA_XOR_START_VALUE;

    for(int i=0;i<size;i++){
        sum += *p;
        xor ^= *p;
        p++;
    }

    uint8_t ret = 0;
    if(CONFIG_DATA_VERSION != est_config->version) ret |= 0x01;
    if(sum                 != est_config->sum)     ret |= 0x02;
    if(xor                 != est_config->xor)     ret |= 0x04;

    if(set){
        est_config->version = CONFIG_DATA_VERSION;
        est_config->sum     = sum;
        est_config->xor     = xor;
    }

    return ret;
}




bool     est_config_storage_read(EstConfig * est_config){
    stored_conf_read((uint8_t *)est_config, sizeof(EstConfig));
    if(config_data_error_check(est_config, false)){
        est_config_default(est_config);
        return false;
    }else{
        return true;
    }
}

void     est_config_storage_write(EstConfig * est_config){
    est_config->version = CONFIG_DATA_VERSION;
    config_data_error_check(est_config, true);
    stored_conf_write((uint8_t *)est_config, sizeof(EstConfig));
}

void est_config_default(EstConfig * est_config){
    est_config->lora_mode = LORA_MODE_LORAWAN_ABP;

    est_config->lora_par.channel        = 8;
    est_config->lora_par.sf             = 7;
    memcpy(est_config->lora_par.device_address, dev_add, 4);

    est_config->lorawan_abp_par.channel        = 8 ;
    est_config->lorawan_abp_par.sf             = 7;
//    est_config->lorawan_abp_par.fcnt           = 0x0123;
    memcpy(est_config->lorawan_abp_par.device_address, dev_add,    4);
    memcpy(est_config->lorawan_abp_par.app_s_key,      app_s_key, 16);   // AppSKey	16 bytes	Chave da aplicação
    memcpy(est_config->lorawan_abp_par.nwk_s_key,      nwk_s_key, 16);   // NwkSKey	16 bytes	Chave de r

    est_config->lorawan_otaa_par.channel = 8;
    est_config->lorawan_otaa_par.sf      = 7;
    memcpy(est_config->lorawan_otaa_par.dev_eui, dev_eui, 8);       // DevEUI	             8 bytes	Identificador único do dispositivo
    memcpy(est_config->lorawan_otaa_par.app_eui, app_eui, 8);       // AppEUI (ou JoinEUI)   8 bytes	Identificador da aplicação
    memcpy(est_config->lorawan_otaa_par.app_key, app_key, 16);      // AppKey	            16 bytes	Chave secreta (AES-128)

    est_config->sleep_time_min = 0;     // teste a cada 10 segundos


//    est_config->active_sensors.battery  = true;
    est_config->active_sensors.bme280   = false;
    est_config->active_sensors.gps      = false;
    est_config->active_sensors.lux      = false;
    est_config->active_sensors.uv       = false;   // not implemented
    est_config->active_sensors.wind     = false;   // not implemented
    est_config->active_sensors.vsys     = true;
    est_config->active_sensors.cpu_temp = true;

    est_config->usb_mode                = USB_MODE_OFF;
    est_config->leds_on                 = false;

    est_config->lux_k_10000             = 10000;    // k = lux_k_10000 / 10000 = 1
    est_config->vsys_k_10000            = 10000;    // k = lux_k_10000 / 10000 = 1

    est_config->always_menu             = false;
}