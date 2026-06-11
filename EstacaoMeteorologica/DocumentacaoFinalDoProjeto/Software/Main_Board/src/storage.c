/**
 * @file    storage.c
 * @author  Carlos e Ricardo
 * @brief   Rotinas para leitura e escrita na Flash
 * Obs.: O outro core tem que estar rodando em RAM, e com suas interrupções paradas
 * Obs.: As rotinas aqui só administram o Core corrente
 * @version 0.1
 * @date    2025-09-16
 * 
 * @copyright Copyright (c) 2025
 */
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/flash.h"
#include "../include/storage.h"


void __not_in_flash_func(stored_conf_clear)() {
//void stored_conf_clear(){
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(STORAGE_OFFSET_CONF, STORAGE_CLEAR_BLOCK_SIZE);
    restore_interrupts(ints);
}

void stored_conf_read(uint8_t *data, int size){
    //memcpy(&config_data, (uint8_t *)(XIP_BASE + STORAGE_OFFSET_CONF), sizeof(config_data));
    memcpy(data, (uint8_t *)(XIP_BASE + STORAGE_OFFSET_CONF), size);
}

void __not_in_flash_func(stored_conf_write)(uint8_t *data, int size){
//void stored_conf_write(){
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(STORAGE_OFFSET_CONF, STORAGE_CLEAR_BLOCK_SIZE);
    //flash_range_program(STORAGE_OFFSET_CONF, (uint8_t *)&config_data, sizeof(config_data));
    flash_range_program(STORAGE_OFFSET_CONF, data, size);
    restore_interrupts(ints);
}

// ---------- Função de escrita na flash (em RAM) ----------
void  __not_in_flash_func(stored_data_clear)(int32_t index){
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(STORAGE_OFFSET_DATA_FIRST + ((index >> 4) * STORAGE_CLEAR_BLOCK_SIZE), STORAGE_CLEAR_BLOCK_SIZE);
    restore_interrupts(ints);
}

void    stored_data_clear_all(){
    printf("stored_data_clear_all\n");
    sleep_ms(200);
     for(int i=0;i<STORAGE_ADDRESS_DATA_N_ITEMS;i+=16){
        stored_data_clear(i);
        printf("Limpando i=%d\n", i);
    }
}

/**
 * @brief Lê uma aquisição especificada pelo seu index e coloca no buffer estation_data
 * 
 * @param index 
 * @param value uint32_t[2] 
 */
void    stored_data_read( int32_t index, uint8_t * value){
    memcpy(value, (const uint8_t *)(XIP_BASE + STORAGE_OFFSET_DATA_FIRST + (index * STORAGE_ADDRESS_DATA_SIZE)), 8);
}

/**
 * @brief Salva o dado na Flash, limpando o bloco se necessário
 * - ATENÇÃO:
 *   -As interrupções de ambos os cores devem estar desabilitadas
 *   -Os COREs só podem estar rodando na RAM
 * @param index 
 * @param estation_data 
 * @param update, quando true ==> não limpa a flash só sobre escreve
 */
void __not_in_flash_func(stored_data_write)(int32_t index, uint8_t * value, bool update){
    uint32_t ints = save_and_disable_interrupts();
    if((!(index & 0x000F))&&(!update)) {
        //stored_data_clear(index);
        flash_range_erase(STORAGE_OFFSET_DATA_FIRST + ((index >> 4) * STORAGE_CLEAR_BLOCK_SIZE), STORAGE_CLEAR_BLOCK_SIZE);
    }

    flash_range_program(STORAGE_OFFSET_DATA_FIRST + (index * STORAGE_ADDRESS_DATA_SIZE), (uint8_t *) value, 256);
    restore_interrupts(ints);
}

