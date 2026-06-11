/**
 * @file    aq_data_lux.c
 * @author  Antonio-Carlos-Ricardo
 * @brief   Driver do sensor de luminosidade BH1750
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "hardware/i2c.h"
#include "../include/aq_data_lux.h"
#include "../include/code_config.h"
#include "../include/loop_printf.h"



#define BH1750_ADDR                 0x23 // ou 0x5C, dependendo do sensor

// Comandos do BH1750
#define BH1750_CMD_POWER_DOWN       0x00
#define BH1750_CMD_POWER_ON         0x01
#define BH1750_CMD_RESET            0x07

#define BH1750_CMD_HRES_CONT        0x10    // Modo contínuo, alta resolução, passo da leitura é 1 em 1
#define BH1750_CMD_HRES2_CONT       0x11    // Modo contínuo, passo da leitura é 1 em 0,5, ou seja, o raw mostra o dobro do BH1750_CMD_HRES_CONT
#define BH1750_CMD_LRES_CONT        0x13    // Modo contínuo, Start measurement at 4lx resolution. Measurement Time is typically 16ms.
                                            // passo da leitura é 4 em 4, "mesma" leitura raw do BH1750_CMD_HRES_CONT

#define BH1750_CMD_HRES_ONETIME    0x20     // Modo OneTime, alta resolução, passo da leitura é 1 em 1
#define BH1750_CMD_HRES2_ONETIME   0x21     // Modo OneTime, passo da leitura é 1 em 0,5, ou seja, o raw mostra o dobro do BH1750_CMD_HRES_ONETIME
#define BH1750_CMD_LRES_ONETIME    0x23     // Modo OneTime, Start measurement at 4lx resolution. Measurement Time is typically 16ms
                                            // passo da leitura é 4 em 4, "mesma" leitura raw do BH1750_CMD_HRES_ONETIME

// Comandos para configurar a largura da janela de aquisição
#define BH1750_CMD_TIME_HIGH        0x40    // 01000 [7,6,5]
#define BH1750_CMD_TIME_LOW         0x60    // 011   [4,3,2,1,0]

// Parâmetros da largura da janela de aquisição
#define BH1750_MTREG                  31    // default = 0x69 = 105

// Tempo, com margem de segurança, para a aquisição
#define BH1750_TIME_TO_READ_US    240000    // Typ = 120ms para MTREG = 0x69 (medido ≃117ms), para MTREG=31 medido ≃ 55ms



static uint16_t aqdatalux_lux_k_10000;
static uint64_t aqdatalux_next_read_us;


int aqdatalux_init_power_on(uint16_t lux_k_10000){
    aqdatalux_lux_k_10000 = lux_k_10000;

    return aqdatalux_init_aq();
}

int aqdatalux_init_aq(){
    uint8_t cmd;

    // Ligar
    cmd = BH1750_CMD_POWER_ON;
    if(i2c_write_timeout_us(I2C_MAIN_BUS, BH1750_ADDR, &cmd, 1, false, I2C_TIMEOUT_US_BH1750) != 1) return -1;
    sleep_us(2000);

    // Reset
    cmd = BH1750_CMD_RESET;
    if(i2c_write_timeout_us(I2C_MAIN_BUS, BH1750_ADDR, &cmd, 1, false, I2C_TIMEOUT_US_BH1750) != 1) return -2;
    sleep_us(2000);

    // Define fator:
    uint8_t cmd1 = 0x40 | (BH1750_MTREG >> 5);      // bits [7:5]
    uint8_t cmd2 = 0x60 | (BH1750_MTREG & 0x1F);    // bits [4:0]

    if(i2c_write_timeout_us(I2C_MAIN_BUS, BH1750_ADDR, &cmd1, 1, false, I2C_TIMEOUT_US_BH1750) != 1) return -3;
    if(i2c_write_timeout_us(I2C_MAIN_BUS, BH1750_ADDR, &cmd2, 1, false, I2C_TIMEOUT_US_BH1750) != 1) return -4;


    // Set mode
    //cmd = BH1750_CMD_HRES_CONT;       // Faz continuamente aquisições,          o que gera um consumo de 115 uA
    cmd = BH1750_CMD_HRES_ONETIME;      // Faz a aquisição e vai para sleep-mode, o que gera um consumo de 3.7 uA
    if(i2c_write_timeout_us(I2C_MAIN_BUS, BH1750_ADDR, &cmd, 1, false, I2C_TIMEOUT_US_BH1750) != 1) return -5;

    aqdatalux_next_read_us = time_us_64() + BH1750_TIME_TO_READ_US;

    return 0;
}

int aqdatalux_read(AqDataLux_Value *value){
    uint8_t data[2];

    while(time_us_64() < aqdatalux_next_read_us);

    value->lux_level_x4 = 0xFFFF;

    if(i2c_read_timeout_us(I2C_MAIN_BUS, BH1750_ADDR, data, 2, false, I2C_TIMEOUT_US_BH1750) == 2){
        uint64_t raw = (data[0] << 8) | data[1];
        if(raw == 0xFFFF){  
            // overflow
            value->lux_level_x4 = 0xFFFF;
        }else{
            raw = (raw * (uint64_t)aqdatalux_lux_k_10000 * 69) / BH1750_MTREG / 12000;
            raw >>= 2;  // divide por 4 pois será transmitido com steps 4lux
            if(raw >= 0x00FFFF) value->lux_level_x4 = 0xFFFF;
                           else value->lux_level_x4 = (uint16_t)raw;

        }

        if(DEBUG_ON_LUXIMETER){
            loop_printf("- Lux Meter          = %d lux\n", (uint32_t)value->lux_level_x4 * 4);
        }
        return 0;
    }else{
        if(DEBUG_ON_LUXIMETER){
            loop_printf("- Lux Meter          = Falha na Leitura do I2C\n");
        }
        return -1;
    }
}

int aqdatalux_sleep(){
    return 0;
}