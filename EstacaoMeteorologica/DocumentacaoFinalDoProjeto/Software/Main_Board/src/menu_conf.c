/**
 * @file    menu_conf.h
 * @author  Antonio-Carlos-Ricardo
 * @brief   Ativa o menu de configuração da estação via USB
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "../include/aq_data.h"
#include "../include/code_config.h"
#include "../include/fcnt.h"
#include "../include/est_config.h"
#include "../include/menu_conf.h"
#include "../include/storage.h"

/**
 * @brief Rotina para mostrar valores hexadecimais
 * 
 * @param array Array de valores
 * @param array_size Número de bytes
 */
static void print_hex_array(uint8_t * array, int array_size){
    for(int i=0;i<array_size;i++){
        printf("%02x ", array[i]);
    }
}

/**
 * @brief Converte um caractere em um valor hexadecimal de 0 a F
 * 
 * @param c Caractere que será convertido
 * @return uint8_t Valor do caractere:
 *         - de 0x00 à 0x0F se for um caractere válido
 *         - 0xFF se for um caractere inválid
 */
static uint8_t char_to_uint8(char c){
    if((c >= '0') && (c <= '9')) return c - '0';
    if((c >= 'A') && (c <= 'F')) return c - 'A' + 10;
    if((c >= 'a') && (c <= 'f')) return c - 'a' + 10;
    return 0xFF;
}

static uint8_t inp_v[32];

/**
 * @brief "Entra" os caracteres digitados em um array de bytes
 * 
 * @param array Array a onde os dados serão gravados
 * @param array_size Tamanho do array
 */
static void input_hex(uint8_t * array, int array_size){
    int pos = 0;
    while(1){
        char c = getchar();
        if(c == '\r') return;
        if(c == '\n') return;
        if(c == 0x7F) { // back space
            if(pos){
                printf("\b");
                pos--;
            }
        }else{
            uint8_t v = char_to_uint8(c);
            if(v != 0xFF){
                inp_v[pos] = v;
                pos++;
                if(pos >= (array_size * 2)){//leu tudo
                    for(int i=0;i<array_size;i++){
                        array[i] = (inp_v[i*2] << 4) | inp_v[i*2+1];
                    }
                    return;
                }else{
                    printf("%c", c);
                }
            }
        }
    }
}


/**
 * @brief "Entra" os caracteres digitados e os transforma em um uint16_t
 * 
 * @return uint16_t Valor correspondente ao valor digitado
 */
static uint16_t input_number(){
    uint16_t ret_val = 0;
    int pos = 0;
    while(1){
        char c = getchar();
        if(c == '\r') return ret_val;
        if(c == '\n') return ret_val;
        if(c == 0x7F) { // back space
            if(pos){
                printf("\b");
                pos--;
                ret_val = ret_val / 10;
            }
        }else{
            if((c >= '0')&&(c <= '9')){
                printf("%c", c);
                uint32_t r = (uint32_t)ret_val * 10 + (c-'0');
                if (r > 0x00FFFF) return 0xFFFF;
                ret_val = (uint16_t)r;
                pos++;
                if(pos >= 5) return ret_val;
            }
        }
    }
}


void menu_conf(EstConfig * est_config, bool valid_flash_data){
    bool fim = false;
    while(!fim){
        printf("\033[2J\033[H");    // Faz com que o terminal volte para o início da tela

        printf("********** %s **********\n\n", NAME);
        printf("Version: %s - %s - Build: %s\n", VERSION, VERSION_DATA, BUILD);

        printf(", Low Power mode: ");
        #if HW_SLEEP_LOW_POWER
        printf("ON");
        #else
        printf("OFF");
        #endif

        printf(" , Debug Probe: ");
        #ifdef PICO_WITH_DEBUG_PROBE
        printf("ON, ");
        #else
        printf("OFF, ");
        #endif

        printf(valid_flash_data?"Flash: Valid Data\n\n\n":"Flash: Invalid Data\n\n\n");
        printf("M) lora Mode: ");
        switch(est_config->lora_mode){
            case LORA_MODE_LORA:
                printf("LoRa\n");
                printf("  H) cHannel        : %02d\n", est_config->lora_par.channel);
                printf("  F) sF             : %02d\n", est_config->lora_par.sf);
                printf("  D) Device address : ");
                print_hex_array(est_config->lora_par.device_address, 4);
                printf("\n\n\n\n");
                break;
            case LORA_MODE_LORAWAN_ABP:
                printf("LoRaWAN ABP\n");
                
                switch(est_config->lorawan_abp_par.channel){
                    case 0xFD: printf("  H) cHannel        : AUTO(0-7)\n");  break;
                    case 0xFE: printf("  H) cHannel        : AUTO(8-15)\n"); break;
                    case 0xFF: printf("  H) cHannel        : AUTO(0-15)\n"); break;
                    default:   printf("  H) cHannel        : %02d\n", est_config->lorawan_abp_par.channel);
                }
                
                if(est_config->lorawan_abp_par.sf      == 0xFF) printf("  F) sF             : AUTO\n");
                                                           else printf("  F) sF             : %02d\n",   est_config->lorawan_abp_par.sf);
                printf("  T) fcnT           : 0x%08x\n", fcnt_get_next());
                printf("  D) Device address : ");
                print_hex_array(est_config->lorawan_abp_par.device_address, 4);
                printf("\n");
                printf("  A) App_s_key      : ");
                print_hex_array(est_config->lorawan_abp_par.app_s_key, 16);
                printf("\n");
                printf("  N) Nwk_s_key      : ");
                print_hex_array(est_config->lorawan_abp_par.nwk_s_key, 16);
                printf("\n");
                break;
            case LORA_MODE_LORAWAN_OTAA:
                printf("LoRaWAN OTAA\n");
                if(est_config->lorawan_otaa_par.channel == 0xFF) printf("  H) cHannel : AUTO\n");
                                                            else printf("  H) cHannel : %02d\n", est_config->lorawan_otaa_par.channel);
                if(est_config->lorawan_otaa_par.sf      == 0xFF) printf("  F) sF      : AUTO\n");
                                                            else printf("  F) sF      : %02d\n", est_config->lorawan_otaa_par.sf);
                printf("  D) Dev eui : ");
                print_hex_array(est_config->lorawan_otaa_par.dev_eui, 8);
                printf("\n");
                printf("  A) App eui : ");
                print_hex_array(est_config->lorawan_otaa_par.app_eui, 8);
                printf("\n");
                printf("  P) aPp key : ");
                print_hex_array(est_config->lorawan_otaa_par.app_key, 16);
                printf("\n\n");
                break;
        }
        printf("\n\n");

        if(est_config->sleep_time_min) printf("S) Sampling time: %d minutes\n\n", est_config->sleep_time_min);
                                  else printf("S) Sampling time: 10 seconds (To Test only)\n\n");

//        printf("1) battery level: %s\n", est_config->active_sensors.battery?"On":"Off");
        printf("2) bme280       : %s\n", est_config->active_sensors.bme280? "On":"Off");
        printf("3) gps          : %s\n", est_config->active_sensors.gps?    "On":"Off");
        printf("4) luximeter    : %s\n", est_config->active_sensors.lux?    "On":"Off");
        printf("   K) factor    : %d (K = %.4f)\n", est_config->lux_k_10000, est_config->lux_k_10000 * 0.0001);
        printf("5) uv index     : NOT implemented\n");
        printf("6) wind         : NOT implemented\n");
        printf("7) vsys         : %s\n", est_config->active_sensors.vsys?   "On":"Off");
        printf("   Q) factor    : %d (K = %.4f)\n", est_config->vsys_k_10000, est_config->vsys_k_10000 * 0.0001);
        printf("8) mpu temp     : %s\n\n", est_config->active_sensors.cpu_temp?"On":"Off");

        if(est_config->leds_on) printf("L) Leds state   : On (not recommended)\n");
                           else printf("L) Leds state   : Off\n");
        switch(est_config->usb_mode){
            case USB_MODE_OFF   : printf("U) Usb : Off\n\n\n");           break;
            case USB_MODE_ON    : printf("U) Usb : On (not recommended)\n\n\n");            break;
            case USB_MODE_OFF_ON: printf("U) Usb : On-Off (not recommended)\n\n\n"); break;
        }

        printf("C) Clear all data\n");
        printf("Y) it alwaYs starts up showing the menu : %s (only for test)\n", est_config->always_menu?"On":"Off");
        printf("R) Reset Configuration\n\n");
        printf("W) Write configuration\n\n");
        printf("X) eXit\n\n");

        switch(toupper(getchar())){
            case 'A':
                switch(est_config->lora_mode){
                    case LORA_MODE_LORA: break;
                    case LORA_MODE_LORAWAN_ABP:
                        printf("LoRaWAN ABP App_s_key : ");
                        input_hex(est_config->lorawan_abp_par.app_s_key, 16);
                        break;
                    case LORA_MODE_LORAWAN_OTAA:
                        printf("LoRaWAN OTAA App EUI : ");
                        input_hex(est_config->lorawan_otaa_par.app_eui, 8);
                        break;
                }
                break;
            case 'C':
                fcnt_clear(0);
                break;
            case 'D':
                switch(est_config->lora_mode){
                    case LORA_MODE_LORA:
                        printf("LoRa Device Address : ");
                        input_hex(est_config->lora_par.device_address, 4);
                        break;
                    case LORA_MODE_LORAWAN_ABP:
                        printf("LoRaWAN ABP Device Address : ");
                        input_hex(est_config->lorawan_abp_par.device_address, 4);
                        break;
                    case LORA_MODE_LORAWAN_OTAA:
                        printf("LoRaWAN OTAA Dev EUI : ");
                        input_hex(est_config->lorawan_otaa_par.dev_eui, 8);
                        break;
                }
                break;
            case 'F':   // sf
                switch(est_config->lora_mode){
                    case LORA_MODE_LORA:
                        if(++est_config->lora_par.sf > 12) est_config->lora_par.sf = 7;
                        break;
                    case LORA_MODE_LORAWAN_ABP:
                        if(est_config->lorawan_abp_par.sf > 12) est_config->lorawan_abp_par.sf = 7;
                        else if(++est_config->lorawan_abp_par.sf > 12) est_config->lorawan_abp_par.sf = 0xFF;
                        break;
                    case LORA_MODE_LORAWAN_OTAA:
                        if(est_config->lorawan_otaa_par.sf > 12) est_config->lorawan_otaa_par.sf = 0x7;
                        else if(++est_config->lorawan_otaa_par.sf > 12) est_config->lorawan_otaa_par.sf = 0xFF;
                        break;
                }
                break;                
            case 'H':   // Channel
                switch(est_config->lora_mode){
                    case LORA_MODE_LORA:
                        if(++est_config->lora_par.channel > 63) est_config->lora_par.channel = 0;
                        break;
                    case LORA_MODE_LORAWAN_ABP:
                    case LORA_MODE_LORAWAN_OTAA:
                        if(est_config->lorawan_abp_par.channel < 63){
                            est_config->lorawan_abp_par.channel++;
                        } else if(est_config->lorawan_abp_par.channel == 63){
                            est_config->lorawan_abp_par.channel = 0xFD;
                        } else if(est_config->lorawan_abp_par.channel == 0xFD){
                            est_config->lorawan_abp_par.channel = 0xFE;
                        } else if(est_config->lorawan_abp_par.channel == 0xFE){
                            est_config->lorawan_abp_par.channel = 0xFF;
                        } else est_config->lorawan_abp_par.channel = 0;;
                        break;
                }
                break;
            case 'K':
                printf("Luximeter factor : ");
                est_config->lux_k_10000 = input_number();
                break;
            case 'L':
                est_config->leds_on = !est_config->leds_on;
                break;
            case 'M':
                //if(++est_config->lora_mode >= LORA_MODE_COUNT) est_config->lora_mode = LORA_MODE_LORAWAN_ABP;
                if(++est_config->lora_mode >= LORA_MODE_COUNT) est_config->lora_mode = LORA_MODE_LORA;
                break;
            case 'N':
                switch(est_config->lora_mode){
                    case LORA_MODE_LORA: break;
                    case LORA_MODE_LORAWAN_ABP:
                        printf("LoRaWAN ABP nwk_s_key : ");
                        input_hex(est_config->lorawan_abp_par.nwk_s_key, 16);
                        break;
                    case LORA_MODE_LORAWAN_OTAA: break;
                }
                break;
            case 'P':
                switch(est_config->lora_mode){
                    case LORA_MODE_LORA: break;
                    case LORA_MODE_LORAWAN_ABP: break;
                    case LORA_MODE_LORAWAN_OTAA:
                        printf("LoRaWAN OTAA App Key : ");
                        input_hex(est_config->lorawan_otaa_par.app_key, 16);
                        break;
                }
                break;
            case 'Q':
                printf("VSys factor : ");
                est_config->vsys_k_10000 = input_number();
                break;
            case 'R': est_config_default(est_config); break;
            case 'S': 
                switch(est_config->sleep_time_min){
                    case  0: est_config->sleep_time_min =  1; break;
                    case  1: est_config->sleep_time_min =  2; break;
                    case  2: est_config->sleep_time_min =  5; break;
                    case  5: est_config->sleep_time_min = 10; break;
                    case 10: est_config->sleep_time_min = 15; break;
                    case 15: est_config->sleep_time_min = 20; break;
                    case 20: est_config->sleep_time_min = 30; break;
                    case 30: est_config->sleep_time_min = 45; break;
                    case 45: est_config->sleep_time_min = 60; break;
                    case 60: est_config->sleep_time_min =  0; break;
                    default: est_config->sleep_time_min =  0; break;
                }
                break;
            case 'T':
                switch(est_config->lora_mode){
                    case LORA_MODE_LORA: break;
                    case LORA_MODE_LORAWAN_ABP:
                        printf("LoRaWAN fcnt : ");
                        uint8_t fcnt[4];
                        //fcnt[0] = (uint8_t)(est_config->lorawan_abp_par.fcnt >> 8);
                        //fcnt[1] = (uint8_t)(est_config->lorawan_abp_par.fcnt & 0X00FF);
                        //input_hex(fcnt, 2);
                        //est_config->lorawan_abp_par.fcnt = ((uint16_t)fcnt[0] << 8) | fcnt[1];
                        //uint16_t n_fcnt = ((uint16_t)fcnt[0] << 8) | fcnt[1];

                        input_hex(fcnt, 4);
                        uint32_t n_fcnt = 0;
                        for(int i=0;i<4;i++){
                            n_fcnt = (n_fcnt << 8) | fcnt[i];
                        }

                        fcnt_clear(n_fcnt);
                        break;
                    case LORA_MODE_LORAWAN_OTAA: break;
                }
                break;

            case 'U':
                if(++est_config->usb_mode >= USB_MODE_COUNT) est_config->usb_mode = 0;
                break;

            
            case 'W': est_config_storage_write(est_config); break;
            case 'X': fim = true; break;
            case 'Y':
                est_config->always_menu = !est_config->always_menu;
                break;                                

//            case '1': est_config->active_sensors.battery  = !est_config->active_sensors.battery;  break;
            case '2': est_config->active_sensors.bme280   = !est_config->active_sensors.bme280;   break;
            case '3': est_config->active_sensors.gps      = !est_config->active_sensors.gps;      break;
            case '4': est_config->active_sensors.lux      = !est_config->active_sensors.lux;      break;
            case '5':                                                                             break;
            case '6':                                                                             break;
            case '7': est_config->active_sensors.vsys     = !est_config->active_sensors.vsys;     break;
            case '8': est_config->active_sensors.cpu_temp = !est_config->active_sensors.cpu_temp; break;
        }
    }   
}