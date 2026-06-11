#include <string.h>
#include "pico/stdlib.h"
#include "../include/aq_data.h"
#include "../include/code_config.h"
#include "../include/fcnt.h"
#include "../include/loop_printf.h"
#include "../include/wcm.h"
#include "../include/wrap_watchdog.h"

/* *********** Os 5 comandos default esperados **********
- LoRa
    CMD: L 08 07 260C293F CF994509EF247AF4976157E876D9C710E1271064FE
    CMD: L 08 07 260C293F CF9A4609F0247BF4976158E876D9C810E22AF865FF
    CMD: L 08 07 260C293F CF9B4709F1247CF4976159E876D9C910E32EE06600
    CMD: L 08 07 260C293F CF9C4809F2247DF497615AE876D9CA10E432C86701
    CMD: L 08 07 260C293F CF9D4909F3247EF497615BE876D9CB10E536B06802

    CMD: L 08 07 260C293F CFFF820A733440F46B8070E893BAC01D5B001BEE1E


- LoRaWAN ABP
    CMD: A 08 07 260C293F 0123 6DEF7D268C810BB9B135F65E0D673D33 4885EA88C4CAB6F824DEFD47C20A1D63 CF994509EF247AF4976157E876D9C710E1271064FE
    CMD: A 08 07 260C293F 0124 6DEF7D268C810BB9B135F65E0D673D33 4885EA88C4CAB6F824DEFD47C20A1D63 CF9A4609F0247BF4976158E876D9C810E22AF865FF
    CMD: A 08 07 260C293F 0125 6DEF7D268C810BB9B135F65E0D673D33 4885EA88C4CAB6F824DEFD47C20A1D63 CF9B4709F1247CF4976159E876D9C910E32EE06600
    CMD: A 08 07 260C293F 0126 6DEF7D268C810BB9B135F65E0D673D33 4885EA88C4CAB6F824DEFD47C20A1D63 CF9C4809F2247DF497615AE876D9CA10E432C86701
    CMD: A 08 07 260C293F 0127 6DEF7D268C810BB9B135F65E0D673D33 4885EA88C4CAB6F824DEFD47C20A1D63 CF9D4909F3247EF497615BE876D9CB10E536B06802

    CMD: A 08 07 260C293F 0128 6DEF7D268C810BB9B135F65E0D673D33 4885EA88C4CAB6F824DEFD47C20A1D63 CFFF830A6B3442F46B8230E893B9001C63001BEB1E


- LoRaWAN OTAA
    CMD: O 08 07 0123456789ABCDEF 0011223344556677 FFEEDDCCBBAA99887766554433221100 CF994509EF247AF4976157E876D9C710E1271064FE
    CMD: O 08 07 0123456789ABCDEF 0011223344556677 FFEEDDCCBBAA99887766554433221100 CF9A4609F0247BF4976158E876D9C810E22AF865FF
    CMD: O 08 07 0123456789ABCDEF 0011223344556677 FFEEDDCCBBAA99887766554433221100 CF9B4709F1247CF4976159E876D9C910E32EE06600
    CMD: O 08 07 0123456789ABCDEF 0011223344556677 FFEEDDCCBBAA99887766554433221100 CF9C4809F2247DF497615AE876D9CA10E432C86701
    CMD: O 08 07 0123456789ABCDEF 0011223344556677 FFEEDDCCBBAA99887766554433221100 CF9D4909F3247EF497615BE876D9CB10E536B06802

    CMD: O 08 07 0123456789ABCDEF 0011223344556677 FFEEDDCCBBAA99887766554433221100 CFFF820A663443F46B81F0E893BA001CBD001BE61F
*/


static uint8_t cmd[256];
static int     cmd_pos = 0;

/**
 * @brief Adiciona um caractere de espaço no fim do string cmd
 * 
 */
static void cmd_add_space(){
    cmd[cmd_pos++] = ' ';
    cmd[cmd_pos] = 0;
}

/**
 * @brief Adiciona o valor hexadecimal (2 caracteres) no final do string cmd
 * 
 * @param dado 
 */
static void cmd_hex_byte(uint8_t dado){
    cmd_pos += sprintf(&cmd[cmd_pos], "%02X", dado);
}

/**
 * @brief Adiciona o valor hexadecimal (2 caracteres) de cada um dos dados do array no final do string cmd
 *        - adiciona um espaço antes dos valores
 * 
 * @param array Dados a serem adicionados
 * @param array_size Quantidade de bytes do array
 */
static void cmd_add_array(uint8_t * array, int array_size){
    cmd_add_space();
    for(int i=0;i<array_size;i++){
        cmd_hex_byte(array[i]);
    }
}



void wcm_init(){
    // Inicializa UART
    uart_init(WCM_UART_ID, WCM_UART_BAUD_RATE);
    gpio_set_function(WCM_UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(WCM_UART_RX_PIN, GPIO_FUNC_UART);
    sleep_ms(10);
}

void wcm_send(EstConfig *est_config, AqData *aq_data){

#ifdef START_SEND_WITH_FIX_DATA
    // Define valores fixos para as 5 primeiras transmissões
    static int count = 0;
    if(count < 5){
//        aq_data->battery.bat_level =        153;    // 76.5 %
        aq_data->bmep280.humidity  =         69;    // 34.5 %
        aq_data->bmep280.temp      =       2543;    // 25.43 Celsius
        aq_data->bmep280.press     =       9338;    // 1013.24 hPa
        aq_data->gps.latitude      = -191405737;    // -22.817341924
        aq_data->gps.longitude     = -394864185;    // -47.071478963
        aq_data->gps.altitude      =       4321;    // 432.1 m
        aq_data->lux.lux_level_x4  =      10000;    // 40000 lux
        aq_data->vsys              =        100;    // 2000 mv
        aq_data->cpu_temp_deci     =        -20;    // -2 C

//        aq_data->battery.bat_level += count;
        aq_data->bmep280.humidity  += count;
        aq_data->bmep280.temp      += count;
        aq_data->bmep280.press     += count;
        aq_data->gps.latitude      += count;
        aq_data->gps.longitude     += count;
        aq_data->gps.altitude      += count;
        aq_data->lux.lux_level_x4  += count * 1000;
        aq_data->vsys              += count;
        aq_data->cpu_temp_deci     += count * 10;

        count++;
    }
#endif

    // ----- Gera comando -----
    switch(est_config->lora_mode){
        case LORA_MODE_LORA:
            // Example:  L ch sf DevAddres -Message\\n\n"
            // Example1: L 05 07 01234567 12345678\n"
            cmd_pos = sprintf(cmd, "L %02x %02x",
                est_config->lora_par.channel,
                est_config->lora_par.sf);
                cmd_add_array(est_config->lora_par.device_address, 4);
            break;
        case LORA_MODE_LORAWAN_ABP:
            // Example  : A ch sf devaddr  fcnt ------------appskey------------- ------------nwkskey------------- -Message--\n
            // Example 1: A 05 07 12345678 87654321 00112233445566778899AABBCCDDEEFF FFEEDDCCBBAA99887766554433221100 0987654321\n
            //cmd_pos = sprintf(cmd, "A %02x %02x %02X%02X%02X%02X %04X",
            #ifdef LORAWAN_ABP_FCNT_4BYTES
            cmd_pos = sprintf(cmd, "A %02x %02x %02X%02X%02X%02X %04X",
                est_config->lorawan_abp_par.channel,
                est_config->lorawan_abp_par.sf,
                est_config->lorawan_abp_par.device_address[0],
                est_config->lorawan_abp_par.device_address[1],
                est_config->lorawan_abp_par.device_address[2],
                est_config->lorawan_abp_par.device_address[3],
                (uint16_t)(fcnt_get_next() & 0x0000FFFF));    
            #else
            cmd_pos = sprintf(cmd, "A %02x %02x %02X%02X%02X%02X %08X",
                est_config->lorawan_abp_par.channel,
                est_config->lorawan_abp_par.sf,
                est_config->lorawan_abp_par.device_address[0],
                est_config->lorawan_abp_par.device_address[1],
                est_config->lorawan_abp_par.device_address[2],
                est_config->lorawan_abp_par.device_address[3],
                fcnt_get_next());
            #endif
            cmd_add_array(est_config->lorawan_abp_par.app_s_key, 16);
            cmd_add_array(est_config->lorawan_abp_par.nwk_s_key, 16);
            break;
        case LORA_MODE_LORAWAN_OTAA:
            // Example: O ch sf -----DeveUi----- -----AppUi------ -------------AppKey------------- -Message--\n"
            // Example: O 05 07 0011223344556677 8899AABBCCDDEEFF FFEEDDCCBBAA99887766554433221100 1234567890\n"
            cmd_pos = sprintf(cmd, "O %02x %02x",
                est_config->lorawan_otaa_par.channel,
                est_config->lorawan_otaa_par.sf);
                cmd_add_array(est_config->lorawan_otaa_par.dev_eui,  8);
                cmd_add_array(est_config->lorawan_otaa_par.app_eui,  8);
                cmd_add_array(est_config->lorawan_otaa_par.app_key, 16);
            break;    
    }

    // Add Space
    cmd_add_space();

    loop_printf("----- Dados da transmissão LoRa -----\n");

    int cmd_payload_first = cmd_pos;

    // ----- Adiciona o payload de dados -----
    // Control byte
    uint8_t control1 = 0;
//    if(aq_data->active_sensors.battery)  control1 |= AQ_ITEM_BAT_VALUE;
    if(aq_data->active_sensors.bme280)   control1 |= AQ_ITEM_BME280;
    if(aq_data->active_sensors.gps)      control1 |= AQ_ITEM_GPS;
    if(aq_data->active_sensors.lux)      control1 |= AQ_ITEM_LUX;
    if(aq_data->active_sensors.uv)       control1 |= AQ_ITEM_UV;
    if(aq_data->active_sensors.wind)     control1 |= AQ_ITEM_WIND;
    if(aq_data->active_sensors.vsys)     control1 |= AQ_ITEM_VSYS;
    if(aq_data->active_sensors.cpu_temp) control1 |= AQ_ITEM_CPU_TEMP;   //AQ_ITEM_TBD;
    cmd_hex_byte(control1);
    loop_printf("- Control Byte       = 0x%02X\n", control1);

    /*
    // Battery Level
    loop_printf("- Battery            = ");
    if(aq_data->active_sensors.battery){
        if(aq_data->battery.bat_level == 0xFF) loop_printf("Sensor ERROR\n");
        else{
            loop_printf("%.1f %%\n", aq_data->battery.bat_level * 0.5);
        }
        cmd_hex_byte(aq_data->battery.bat_level);
    }else loop_printf("NO Sensor\n");
    */

    // BMEP280
    if(aq_data->active_sensors.bme280){
        loop_printf("- BME280 Humidity    = ");
        if(aq_data->bmep280.humidity == 0xFF) loop_printf("Sensor ERROR\n");
        else{
            loop_printf("%.1f %%\n", aq_data->bmep280.humidity * 0.5);
        }
        cmd_hex_byte(aq_data->bmep280.humidity);

        loop_printf("- BME280 Temperature = ");
        if(aq_data->bmep280.temp == 0x7FFF) loop_printf("Sensor ERROR\n");
        else{
            loop_printf("%.2f Celsius\n", aq_data->bmep280.temp * 0.01);
        }
        cmd_hex_byte(aq_data->bmep280.temp >> 8); 
        cmd_hex_byte(aq_data->bmep280.temp & 0x00FF);

        loop_printf("- BME280 Pressure    = ");
        if(aq_data->bmep280.press == 0xFFFF) loop_printf("Sensor ERROR\n");
        else{
            loop_printf("%.2f hPa\n", (60000 - aq_data->bmep280.press) * 0.02);
        }
        cmd_hex_byte(aq_data->bmep280.press >> 8); 
        cmd_hex_byte(aq_data->bmep280.press & 0x00FF);
    }else loop_printf("- BME280             = NO Sensor\n");

    // GPS
    if(aq_data->active_sensors.gps){
        loop_printf("- GPS latitude       = ");
        if(aq_data->gps.latitude == 0x7FFFFFFF) loop_printf("Sensor ERROR\n");
        else{
            loop_printf("%f degrees\n", aq_data->gps.latitude / 8388608.0);
        }
        cmd_hex_byte( aq_data->gps.latitude >> 24);
        cmd_hex_byte((aq_data->gps.latitude >> 16) & 0x000000FF);  
        cmd_hex_byte((aq_data->gps.latitude >>  8) & 0x000000FF);  
        cmd_hex_byte( aq_data->gps.latitude        & 0x000000FF);

        loop_printf("- GPS longitude      = ");
        if(aq_data->gps.longitude == 0x7FFFFFFF) loop_printf("Sensor ERROR\n");
        else{
            loop_printf("%f degrees\n", aq_data->gps.longitude / 8388608.0);
        }
        cmd_hex_byte( aq_data->gps.longitude >> 24);
        cmd_hex_byte((aq_data->gps.longitude >> 16) & 0x000000FF);  
        cmd_hex_byte((aq_data->gps.longitude >>  8) & 0x000000FF);  
        cmd_hex_byte( aq_data->gps.longitude        & 0x000000FF);
        
        loop_printf("- GPS altitude       = ");
        if(aq_data->gps.altitude == 0x7FFF) loop_printf("Sensor ERROR\n");
        else{
            loop_printf("%.1f meters\n", aq_data->gps.altitude * 0.1);
        }
        cmd_hex_byte(aq_data->gps.altitude >> 8); 
        cmd_hex_byte(aq_data->gps.altitude & 0x00FF);
    }else loop_printf("- GPS                = NO Sensor\n");

    // Lux-meter
    loop_printf("- Lux Meter          = ");
    if(aq_data->active_sensors.lux){
        if(aq_data->lux.lux_level_x4 == 0xFFFF) loop_printf("Sensor ERROR\n");
        else{
            loop_printf("%u lux\n", aq_data->lux.lux_level_x4 * 4);
        }
        cmd_hex_byte(aq_data->lux.lux_level_x4 >> 8); 
        cmd_hex_byte(aq_data->lux.lux_level_x4 & 0x00FF);
    }else loop_printf("NO Sensor\n");

    // VSys
    loop_printf("- VSys               = ");
    if(aq_data->active_sensors.vsys){
        if(aq_data->vsys == 0xFF) loop_printf("Sensor ERROR\n");
        else{
            loop_printf("%4d mV\n", aq_data->vsys * 20);
        }
        cmd_hex_byte(aq_data->vsys);
    }else loop_printf("NO Sensor\n");

    // MPU Temp
    loop_printf("- MPU Tempetature    = ");
    if(aq_data->active_sensors.cpu_temp){
        if(aq_data->cpu_temp_deci == 0x7F) loop_printf("Sensor ERROR\n");
        else{
            loop_printf("%.1f Celsius\n", aq_data->cpu_temp_deci * 0.1);
        }
        //cmd_hex_byte((aq_data->cpu_temp_deci / 5));
        cmd_hex_byte((int8_t)(aq_data->cpu_temp_deci * 0.1));
    }else loop_printf("NO Sensor\n");


    // Add CHECKSUM
    #ifdef ADD_CHECKSUM_ON_PAYLOAD
        uint8_t sum = 0xA5;
        uint8_t xor = 0x5A;

        for(int i=cmd_payload_first;i < cmd_pos; i++){
            sum += cmd[i];
            xor ^= cmd[i];
        }

        cmd_hex_byte(sum);
        cmd_hex_byte(xor);
    #endif

    // fim de linha
    cmd[cmd_pos++] = '\n';

    // fim de string
    cmd[cmd_pos++] = 0;

    // Comand gerado
    loop_printf("\nCMD: %s\n", cmd);



    // ----- Envio e resposta do comando -----

    // Acordar WCM
    uart_putc_raw(WCM_UART_ID, ' ');    // 0x3F == ?
    sleep_ms(100);

    sleep_ms(200);
    
    //limpar UART do WCM
    while(uart_is_readable(WCM_UART_ID)){
        uart_getc(WCM_UART_ID);
        sleep_us(100);
    }
    
    // Enviar o Comando
    uart_write_blocking(WCM_UART_ID, cmd, strlen(cmd));
    //uart_putc_raw(WCM_UART_ID, '\n');
    loop_printf("Enviou comando\n");


    uint64_t t_timeout = time_us_64() + WCM_TIMEOUT_US;
    while((!uart_is_readable(WCM_UART_ID))&&(t_timeout > time_us_64())){
        wrap_watchdog_update();
    }
    if(uart_is_readable(WCM_UART_ID)){
        char ret_c = uart_getc(WCM_UART_ID);
        loop_printf("Teve resposta: %c\n", ret_c);
    }else{
        loop_printf("Time OUT\n");
    }
    
    loop_printf("----- Fim da transmissão LoRa -----\n\n");    
}