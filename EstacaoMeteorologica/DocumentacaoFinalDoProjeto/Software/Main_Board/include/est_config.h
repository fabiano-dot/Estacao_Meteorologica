/**
 * @file    est_config.h
 * @author  Antonio-Carlos-Ricardo
 * @brief   Define a estrutura de configuração e as manipula
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef EST_CONFIG_H
#define EST_CONFIG_H

#include "pico/stdlib.h"

/**
 * @brief Modos LoRa válidos
 * 
 */
typedef enum{
    LORA_MODE_LORA = 0,
    LORA_MODE_LORAWAN_ABP,
    LORA_MODE_LORAWAN_OTAA,
    LORA_MODE_COUNT
}LoraMode;

/**
 * @brief Parâmetros utilizados para o modo LoRa
 * 
 */
typedef struct {
    uint8_t  channel;
    uint8_t  sf;
    uint8_t  device_address[4];
}LoraPar;

/**
 * @brief Parâmetros utilizados para o modo LoRaWAN ABP
 * 
 */
typedef struct {
    uint8_t  channel;           // 0xFF ==> AUTO
    uint8_t  sf;                // 0xFF ==> AUTO
//    uint16_t fcnt;
    uint8_t  device_address[4];    // DevAddr	4 bytes     Endereço do dispositivo
    uint8_t  app_s_key[16];     // AppSKey	16 bytes	Chave da aplicação
    uint8_t  nwk_s_key[16];     // NwkSKey	16 bytes	Chave de rede
}LorawanAbpPar;

/**
 * @brief Parâmetros utilizados para o modo LoRaWAN OTAA
 * 
 */
typedef struct {
    uint8_t  channel;           // 0xFF ==> AUTO
    uint8_t  sf;                // 0xFF ==> AUTO
    uint8_t  dev_eui[8];        // DevEUI	             8 bytes	Identificador único do dispositivo
    uint8_t  app_eui[8];        // AppEUI (ou JoinEUI)   8 bytes	Identificador da aplicação
    uint8_t  app_key[16];       // AppKey	            16 bytes	Chave secreta (AES-128)
}LorawanOtaaPar;


/**
 * @brief Modo que a USB vai funcionar quando a estação estiver funcionando
 *        depois do menu de configuração
 * 
 */
typedef enum{
    USB_MODE_OFF = 0,
    USB_MODE_ON,
    USB_MODE_OFF_ON,
    USB_MODE_COUNT
}UsbMode;

/**
 * @brief Lista de sensores ativos
 * 
 */
typedef struct {
//    bool battery;
    bool bme280;
    bool gps;
    bool lux;
    bool uv;        // not implemented
    bool wind;      // not implemented
    bool vsys;
    bool cpu_temp;
}ActiveSensors;


/**
 * @brief Definição da variável que abrigará toda a configuração
 * 
 */
typedef struct {
    uint32_t       version;

    LoraMode       lora_mode;
    LoraPar        lora_par;
    LorawanAbpPar  lorawan_abp_par;
    LorawanOtaaPar lorawan_otaa_par;
    uint16_t       sleep_time_min; // Tempo que a estação dorme em minutos, 0 ==> 10 segundo usado para testes
    ActiveSensors  active_sensors;
    UsbMode        usb_mode;
    bool           leds_on;
    uint16_t       lux_k_10000;
    uint16_t       vsys_k_10000;
    bool           always_menu;

    uint32_t       sum;
    uint32_t       xor;
}EstConfig;


/**
 * @brief Lê a configuração que está na memória flash.
 * 
 * @param est_config Variável em que serão colocados os dados
 * @return true Conseguiu ler e os dados são válidos
 * @return false Não conseguiu ler ou os dados são inválidos
 */
bool     est_config_storage_read(EstConfig * est_config);

/**
 * @brief Salva a configuração na memória flash.
 * 
 * @param est_config Variável em que estão os dados
 */
void     est_config_storage_write(EstConfig * est_config);


/**
 * @brief Carrega a variável com valores default
 * 
 * @param est_config Variável em que serão colocados os dados
 */
void est_config_default(EstConfig * est_config);


#endif // EST_CONFIG_H
