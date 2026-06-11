/**
 * @file    main.c
 * @author  Antonio-Carlos-Ricardo
 * @brief   Inicialização do sistema e main loop
 * @version 0.2
 * @date    2026-02-08
 * 
 * @copyright Copyright (c) 2026
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "include/aq_data.h"
#include "include/buttons_and_leds.h"
#include "include/code_config.h"
#include "include/fcnt.h"
#include "include/est_config.h"
#include "include/hw_sleep.h"
#include "include/loop_printf.h"
#include "include/menu_conf.h"
#include "include/wcm.h"
#include "include/wrap_watchdog.h"


#define MAIN_ST_INIT            LED_GREEN       // LED Green, indica inicializando o sistema, tempo para conectar USB
#define MAIN_ST_WAIT_BUTTON     LED_YELLOW      // LED Red e Green, indica que botão será reconhecido
#define MAIN_ST_MENU            LED_RED         // Indica que o menu de configuração esta ativo
#define MAIN_ST_MENU_AFTER      LED_CYAN        // Indica que a estação já passo pelo menu
#define MAIN_ST_AQUISITION      LED_BLUE        // Indica que está fazendo aquisição dos dados
#define MAIN_ST_TRANSMISSION    LED_WHITE       // Indica que está transmitindo os dados
#define MAIN_ST_SLEEPING        LED_BLACK       // Indica que a estação está em sleep-mode, baixo consumo de energia


static uint32_t  wakeup_cycle = 0;
static AqData    aq_data;
static EstConfig est_config;

/**
 * @brief Inicializa todo o sistema e mantêm o main loop
 * 
 * @return int  Nunca retorna (main-loop)
 */
int main(){
    int ret;

    stdio_init_all();
    wrap_watchdog_disable();

    #ifdef SLEEP_STATE_BY_GPIO
        // Configuração das GPIOs utilizadas para monitorar o sleep state da estação
        gpio_set_dir(     SLEEP_STATE_GPIO_0, GPIO_OUT);
        gpio_put(         SLEEP_STATE_GPIO_0, 0);
        gpio_set_function(SLEEP_STATE_GPIO_0, GPIO_FUNC_SIO);

        gpio_put(         SLEEP_STATE_GPIO_1, 0);
        gpio_set_dir(     SLEEP_STATE_GPIO_1, GPIO_OUT);
        gpio_set_function(SLEEP_STATE_GPIO_1, GPIO_FUNC_SIO);
    #endif

#ifdef PICO_WITH_DEBUG_PROBE
    // Reconfiguração dos GPIOs da UART para uso em Debug
    gpio_set_function( 0, GPIO_FUNC_SIO);
    gpio_set_function( 1, GPIO_FUNC_SIO);
    gpio_set_function(17, GPIO_FUNC_UART);
    gpio_set_function(16, GPIO_FUNC_UART);
#endif

    buttons_and_leds_init();
    buttons_and_leds_set_color(MAIN_ST_INIT);
    fcnt_init();
    wcm_init();

    // Dar tempo para conectar a interface serial
    sleep_ms(INITIAL_DELAY_MS);    

    printf("********** %s **********\n\n", NAME);
    printf("Version: %s - %s - Build: %s\n", VERSION, VERSION_DATA, BUILD);

    // Janela para pressionar o botão
    bool button_pressed = false;
    for(int i=0;i<25;i++){
        buttons_and_leds_set_color(i&0x01?MAIN_ST_INIT:MAIN_ST_WAIT_BUTTON);
        for(int j=0;j<100;j++){
            if(buttons_and_leds_button_pressed()) button_pressed = true;
            sleep_us(2000);
        }
    }

    
    bool valid_flash_data = est_config_storage_read(&est_config);
    printf(valid_flash_data?"Flash: Valid Data\n\n\n":"Flash: Invalid Data\n\n\n");

    // Verifica condições para iniciar o terminal TXT:
    //    - Dados inválidos na flash                     ou
    //    - (Botão pressionado  e always_menu == false)  ou
    //    - (Botão solto        e always_menu == true )
    //    Obs.: O Botão pressionado inverte o funcionamento do parâmetro always_menu
    if((!valid_flash_data) || 
       (  button_pressed  && (!est_config.always_menu)) || 
       ((!button_pressed) &&   est_config.always_menu)){
        buttons_and_leds_set_color(MAIN_ST_MENU);
        menu_conf(&est_config, valid_flash_data);
    }

    buttons_and_leds_set_color(MAIN_ST_MENU_AFTER);
    aq_data.active_sensors = est_config.active_sensors;

    // Inicializa, power_on de todos os sensores
    if(ret = aqdata_init_power_on(&aq_data, est_config.lux_k_10000, est_config.vsys_k_10000)){
        loop_printf("Init Power sensors fail: %d\n", ret);
        while(1){
            buttons_and_leds_set_color(LED_BLACK);
            sleep_ms(200);
            buttons_and_leds_set_color(LED_RED);
            sleep_ms(200);
        }
    }

    // desativa recursos desnecessários do rp2040
    hw_sleep_init(est_config.usb_mode, est_config.active_sensors.cpu_temp || est_config.active_sensors.vsys,  est_config.sleep_time_min);    
    buttons_and_leds_set_color(LED_BLACK);
    
    wrap_watchdog_enable();

    while (true) {
        wrap_watchdog_update();
        
        // Avisa que vai iniciar a aquisição
        if(est_config.leds_on) buttons_and_leds_set_color(MAIN_ST_AQUISITION);
        
        // Faz a aquisição completa:
        
        // inicia o processo de aquisição
        if(ret = aqdata_init_aq(&aq_data)){
            loop_printf("Init Aq sensors fail: %d\n", ret);
        }
        // lê os dados
        if(ret = aqdata_read(&aq_data)){
            loop_printf("Read sensors fail: %d\n", ret);
        }
        // Coloca os sensores em sleep-mode
        if(ret = aqdata_sleep(&aq_data)){
            loop_printf("Sleep sensors fail: %d\n", ret);
        }
        
        // Envia os dados
        loop_printf("Station sending data\n");
        if(est_config.leds_on) buttons_and_leds_set_color(MAIN_ST_TRANSMISSION);
        wcm_send(&est_config, &aq_data);

        // salva o último fcnt
        fcnt_update();
        
        // Coloca o rp2040 em "sleep-mode" e o ativa para o modo normal quando for fazer o novo ciclo de leitura e aquisição
        loop_printf("Station sleeping\n");
        if(est_config.leds_on) buttons_and_leds_set_color(MAIN_ST_SLEEPING);
        hw_sleep();

        loop_printf("Station Wake Up %d\n", ++wakeup_cycle);
    }
}