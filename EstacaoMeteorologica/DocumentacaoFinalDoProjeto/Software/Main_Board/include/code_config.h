/**
 * @file    code_config.h
 * @author  Antonio-Carlos-Ricardo
 * @brief   Parâmetros de configuração de compilação
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef CODE_CONFIG_H
#define CODE_CONFIG_H


// ********** Descrição do código gerado **********

#define NAME                        "Weather Station"
#define VERSION                     "0.00.02"
#define VERSION_DATA                "2026/04/07"
#define BUILD                       "0042"
#define CONFIG_DATA_VERSION         5   // uint32_t version;



// ********** Principais parâmetros de configuração **********

// #define PICO_WITH_DEBUG_PROBE                         // habilita o uso do debug probe reconfigurando as portas UART a serem utilizadas

#define DEBUG_ON                false   // true         // habilita printf de log para depuração
#define DEBUG_ON_AD             (DEBUG_ON && true)      // habilita printf de log do módulo aq_data_ad, VSys e MPU Temp
#define DEBUG_ON_BAT            (DEBUG_ON && true)      // habilita printf de log do módulo aq_data_bat
#define DEBUG_ON_BMEP280        (DEBUG_ON && false)     // habilita printf de log do módulo aq_data_bmep280
#define DEBUG_ON_GPS            (DEBUG_ON && true)      // habilita printf de log do módulo aq_data_gps
#define DEBUG_ON_LUXIMETER      (DEBUG_ON && true)      // habilita printf de log do módulo aq_data_lux

#define WATCHDOG_ENABLED                                // se definido ativa o funcionamento do watchdog
// #define START_SEND_WITH_FIX_DATA                      // se definido faz com que as 5 primeiras transmissões sejam valores conhecidos independentes do valor lido, útil para debug
#define SLEEP_STATE_BY_GPIO                             // se definido faz que durante os períodos de sleep os gpio reflitam o funcionamento do Sleep mode, util para debug
#define INITIAL_DELAY_MS        10000                   // define o tempo, em milissegundos, de espera inicial para rodar o programa, permitindo assim conectar a interface serial (USB) antes do programa iniciar
#define USB_TURN_ON_TIME_S      5
// #define ADD_CHECKSUM_ON_PAYLOAD                       // adiciona um CHECKSUM no final do payload
#define LORAWAN_ABP_FCNT_4BYTES


// ********** Parâmetros auxiliares **********

#define LOOP_PRINTF_ENABLED                             // se definido ativa o funcionamento da função loop_printf

// HW_SLEEP_LOW_POWER define se o modo low power será ativado ou não
#ifdef PICO_WITH_DEBUG_PROBE
//#define HW_SLEEP_LOW_POWER false
#define HW_SLEEP_LOW_POWER true
#else
#define HW_SLEEP_LOW_POWER true
#endif



// GPIOs utilizadas para reportar sleep-mode 
#define SLEEP_STATE_GPIO_0      18
#define SLEEP_STATE_GPIO_1      19

// Parâmetros de comunicação do GPS
#define GPS_UART_ID             uart1
#define GPS_UART_UART_IRQ       UART1_IRQ
#define GPS_UART_BAUD_RATE      9600
#define GPS_UART_TX_PIN         8
#define GPS_UART_RX_PIN         9
#define GPS_TIMEOUT_US          10000000

#define GPIO_BUTTON             5
#define GPIO_LED_RED            13
#define GPIO_LED_GREEN          11 
#define GPIO_LED_BLUE           12

// Parâmetros de comunicação do I2C
#define I2C_MAIN_GPIO_SCL       2
#define I2C_MAIN_GPIO_SDA       3
#define I2C_MAIN_BAUDRATE       400000
#define I2C_MAIN_BUS            i2c1
#define I2C_TIMEOUT_US_BMEP280  10000
#define I2C_TIMEOUT_US_BH1750   10000

// Parâmetros de comunicação do WCM via UART
#define WCM_UART_ID             uart0
#define WCM_UART_TX_PIN         0       
#define WCM_UART_RX_PIN         1       
#define WCM_UART_BAUD_RATE      115200
#define WCM_TIMEOUT_US          10000000

#define WATCHDOG_TIME_MS        5000        // dispara o watchdog em 5 segundos se ele não for atualizado


#endif // CODE_CONFIG_H