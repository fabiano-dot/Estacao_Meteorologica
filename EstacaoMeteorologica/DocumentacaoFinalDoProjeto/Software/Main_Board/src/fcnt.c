#include "pico/stdlib.h"
#include "../include/fcnt.h"
#include "../include/storage.h"

static uint32_t next_fcnt;
static int32_t  next_fcnt_index;

void     fcnt_init(){
    uint32_t buffer[2];

    next_fcnt       = 0;
    next_fcnt_index = 0;

    for(int i=0; i< STORAGE_ADDRESS_DATA_N_ITEMS; i++){
        stored_data_read( i, (uint8_t *) buffer);
        if((buffer[0] ^ buffer[1]) == 0xFFFFFFFF){
            if(next_fcnt < buffer[0]){
                next_fcnt       = buffer[0];
                next_fcnt_index = i;
            }
        }
    }
}
void     fcnt_clear(   uint32_t next_value){
    uint32_t buffer[64];

    for(int i=0;i<64;i++) buffer[i] = 0xFFFFFFFF;

    next_fcnt       = next_value;
    next_fcnt_index = 0;
    
    stored_data_clear_all();

    buffer[0] = next_fcnt;
    buffer[1] = next_fcnt  ^ 0xFFFFFFFF;
    stored_data_write(next_fcnt_index, (uint8_t *)buffer, false);
}
void     fcnt_update(){
    uint32_t buffer[64];

    for(int i=0;i<64;i++) buffer[i] = 0xFFFFFFFF;

    next_fcnt++;
    next_fcnt_index++;

    if(next_fcnt_index >= STORAGE_ADDRESS_DATA_N_ITEMS) next_fcnt_index = 0;
    buffer[0] = next_fcnt;
    buffer[1] = next_fcnt  ^ 0xFFFFFFFF;
    stored_data_write(next_fcnt_index, (uint8_t *)buffer, false);
}
uint32_t fcnt_get_next(){
    return next_fcnt;
}