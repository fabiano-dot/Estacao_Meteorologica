/**
 * @file    aq_data_gps.c
 * @author  Antonio-Carlos-Ricardo
 * @brief   Driver do módulo de GPS neo6mv
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <string.h>
#include "../include/aq_data_gps.h"
#include "../include/code_config.h"
#include "../include/loop_printf.h"
#include "../include/wrap_watchdog.h"
//#include "gps/gy_neo6mv2.h"


#define GPS_NMEA_GPGGA_MINIMUM_SIZE     24  // $GPGGA,,,,,,,,,,,,,,*47<cr>
#define GPS_BUFFER_SIZE 128


typedef enum {
    GPS_GGA_NO_ERROR                =  0,
    GPS_GGA_ERROR_SIZE              = -1,
    GPS_GGA_ERROR_NO_NMEA           = -2,
    GPS_GGA_ERROR_TITLE_INVALID     = -3,
    GPS_GGA_ERROR_CHECKSUM_INVALID  = -4,
    GPS_GGA_ERROR_INVALID_N_ITENS   = -5,
    GPS_GGA_ERROR_NO_FIX            = -6,
    GPS_GGA_ERROR_NO_SIGNAL         = -7
} GpsGgaError;



static       char    gps_buffer_int[GPS_BUFFER_SIZE];
static       char    gps_buffer_out[GPS_BUFFER_SIZE];
static       int     gps_buffer_int_idx = 0;
static       int     gps_received_lines = 0;
static const uint8_t nmea_disable[] = {0x01, 0x02, 0x03, 0x04, 0x05};



// ISR da UART
static void gps_isr_uart_rx() {
//void __not_in_flash_func(gps_isr_uart_rx)() {    
    while (uart_is_readable(GPS_UART_ID)) {
        uint8_t ch = uart_getc(GPS_UART_ID);

        if(gps_buffer_int_idx >= (GPS_BUFFER_SIZE - 1)){   // estourou o buffer, recomeçar
            gps_buffer_int_idx = 0;
        }else{
            gps_buffer_int[gps_buffer_int_idx] = ch;
            if(ch =='\n'){
                gps_buffer_int[gps_buffer_int_idx-1] = 0;   // remove o <CR> 
                strcpy(gps_buffer_out, gps_buffer_int);
                gps_received_lines++;
                gps_buffer_int_idx = 0;
            }else{
                gps_buffer_int_idx++;
            }
        }
    }
}

static void gps_ubx_sum(uint8_t * data, uint8_t size){
    uint8_t ck_a = 0;
    uint8_t ck_b = 0;

    for(int i=2;i<(size-2);i++){
        ck_a += data[i];
        ck_b += ck_a;
    }
    data[size - 2] = ck_a;
    data[size - 1] = ck_b;
}

static void gps_disable(uint8_t nmea_f0_id){
    uint8_t disable[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    disable[7] = nmea_f0_id;
    gps_ubx_sum(disable, sizeof(disable));
    uart_write_blocking(GPS_UART_ID, "\r\n\r\n", 4);            // Remover lixos
    uart_write_blocking(GPS_UART_ID, disable, sizeof(disable));
}

static bool gps_check_nmea_checksum(uint8_t * buffer){
    uint8_t sum = 0;
    uint8_t end = strlen(buffer) - 3;
    for(int i=1;i<end;i++){ // xor de todos os itens entre $ e *, excluido o $ e o *
        sum ^= buffer[i];
    }
    char s_sum[3];
    sprintf(s_sum, "%02X", sum);
    return strcasecmp(s_sum, &(buffer[end + 1])) == 0;
}

GpsGgaError gps_read(int32_t * lat, int32_t * lon, int32_t * alt){
    char *token_NMEA;
    char *token_time;
    char *token_lat;
    char *token_lat_ind;
    char *token_long;
    char *token_long_ind;
    char *token_fix;
    char *token_n_sate;
    char *token_hdop;
    char *token_alti;

    float   f_lat, f_lon, f_alt;
    int     tmp;
    int     error = 0;

    *lat = 0xFFFFFFFF;
    *lon = 0xFFFFFFFF;
    *alt = 0xFFFFFFFF;

    irq_set_enabled(GPS_UART_UART_IRQ, false);

    if(DEBUG_ON_GPS) {
        loop_printf("Line=%d, %s\n", gps_received_lines, gps_buffer_out);
    }

    
    if(!error){
        // valida tamanho mínimo de um gga
        if(strlen(gps_buffer_out) < GPS_NMEA_GPGGA_MINIMUM_SIZE) error = GPS_GGA_ERROR_SIZE;
    }

    if(!error){
        // Começa com $
        if(gps_buffer_out[0] != '$') error = GPS_GGA_ERROR_NO_NMEA;
    }

    if(!error){
        // Valida NMEA checksum
        if(!gps_check_nmea_checksum(gps_buffer_out)) error = GPS_GGA_ERROR_CHECKSUM_INVALID;
    }

    if(!error){      
        if(DEBUG_ON_GPS) loop_printf("GPS NMEA: %s\n", gps_buffer_out);
 
        // separa itens
        token_NMEA = strtok(gps_buffer_out, ",");
        token_time     = strtok(NULL,   ",");
        token_lat      = strtok(NULL,   ",");
        token_lat_ind  = strtok(NULL,   ",");
        token_long     = strtok(NULL,   ",");
        token_long_ind = strtok(NULL,   ",");
        token_fix      = strtok(NULL,   ",");
        token_n_sate   = strtok(NULL,   ",");    
        token_hdop     = strtok(NULL,   ",");
        token_alti     = strtok(NULL,   ",");
        if(token_alti == NULL) error = GPS_GGA_ERROR_INVALID_N_ITENS;
    }

    if(!error){        
        // valida se é GGA
        if (!strstr(token_NMEA, "$GPGGA")) error = GPS_GGA_ERROR_TITLE_INVALID;
    }

    if(!error){        
        // valida FIX
        if(*token_fix != '1') error = GPS_GGA_ERROR_NO_FIX;
    }

    if(!error){
        // read latitude
        tmp = ((token_lat[0] - '0') * 10) + (token_lat[1] - '0');
        if(sscanf(&(token_lat[2]),  "%f", &f_lat) != 1) error |= 0x01;
        else{
            f_lat = tmp + f_lat / 60.0;
        }
        switch(*token_lat_ind){
            case 'S':
            case 's':
                if(!(error & 0x01)) f_lat = -f_lat;
                break;
            case 'N':
            case 'n':
                break;
            default:
                error |= 0x02;
        }
    }

    if(!error){        
        // read longitude
        tmp = ((token_long[0] - '0') * 100) + ((token_long[1] - '0') * 10) + (token_long[2] - '0');
        if(sscanf(&(token_long[3]),  "%f", &f_lon) != 1) error |= 0x04;
        else{
            f_lon = tmp + f_lon / 60.0;
        }
        switch(*token_long_ind){
            case 'W':
            case 'w':
                if(!(error & 0x04)) f_lon = -f_lon;
                break;
            case 'E':
            case 'e':
                break;
            default:
                error |= 0x08;
        }
    }

    if(!error){        
        // read altitude
        if(sscanf(token_alti, "%f", &f_alt) != 1) error |= 0x10;
    }



    irq_set_enabled(GPS_UART_UART_IRQ, true);

    if(error) return error;

    *lat = f_lat * 0x00800000;
    *lon = f_lon * 0x00800000;
    *alt = f_alt * 0x00010000;

    if(DEBUG_ON_GPS) {
        loop_printf("\n");
        loop_printf("token_NMEA    :%s\n", token_NMEA    );
        loop_printf("token_time    :%s\n", token_time    );
        loop_printf("token_lat     :%s\n", token_lat     );
        loop_printf("token_lat_ind :%s\n", token_lat_ind );
        loop_printf("token_long    :%s\n", token_long    );
        loop_printf("token_long_ind:%s\n", token_long_ind);
        loop_printf("token_fix     :%s\n", token_fix     );
        loop_printf("token_n_sate  :%s\n", token_n_sate  );
        loop_printf("token_hdop    :%s\n", token_hdop    );
        loop_printf("token_alti    :%s\n", token_alti    );
        
        loop_printf("f_lat : %f\n", f_lat    );
        loop_printf("f_lon : %f\n", f_lon    );
        loop_printf("f_alt : %f\n", f_alt    );
        
        loop_printf("lat : %ld\n", *lat    );
        loop_printf("lon : %ld\n", *lon    );
        loop_printf("alt : %ld\n", *alt    );
    }

    return GPS_GGA_NO_ERROR;    // Sinal é válido
}

















int aqdatagps_init_power_on(){
    if(DEBUG_ON_GPS) loop_printf("gps_init()\n");

    // Inicializa UART
    uart_init(GPS_UART_ID, GPS_UART_BAUD_RATE);
    gpio_set_function(GPS_UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(GPS_UART_RX_PIN, GPIO_FUNC_UART);
    sleep_ms(100);

    for(int i = 0;i<sizeof(nmea_disable);i++){
        gps_disable(nmea_disable[i]);
        sleep_ms(10);
    }

    return 0;
}

int aqdatagps_init_aq(){
    // limpa uart buffer;
    while (uart_is_readable(GPS_UART_ID)) uart_getc(GPS_UART_ID);
    gps_buffer_out[0] = 0;

    // Habilita interrupção de RX
    //gps_buffer_int_idx = 0;
    //gps_received_lines = 0;
    irq_set_exclusive_handler(GPS_UART_UART_IRQ, gps_isr_uart_rx);
    irq_set_enabled(          GPS_UART_UART_IRQ, true);
    uart_set_irq_enables(     GPS_UART_ID, true, false);  // RX on, TX off

    return 0;
}

int aqdatagps_read(AqDataGps_Value *value){
    int32_t lat, lon, alt;

    uint64_t t0 = time_us_64() + GPS_TIMEOUT_US;
    GpsGgaError ret;
    bool fim = false;
    while(!fim){
        wrap_watchdog_update();
        ret = gps_read(&lat, &lon, &alt);
        if(!ret) fim = true;
        else{
            if(time_us_64() > t0) fim = true;
        }

    }
    if(ret){
        value->altitude  = 0x7FFF;
        value->latitude  = 0x7FFFFFFF;
        value->longitude = 0x7FFFFFFF;
    }else{
        value->altitude  = alt / 6553.6;
        value->latitude  = lat;
        value->longitude = lon;
    }
    
    return 0;
}
int aqdatagps_sleep(){
    return 0;
}