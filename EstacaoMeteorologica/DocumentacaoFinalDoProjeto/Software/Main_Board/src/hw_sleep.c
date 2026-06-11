#include <stdio.h>
#include "hardware/adc.h"
#include "hardware/clocks.h" 
#include "hardware/pll.h" 
#include "hardware/regs/usb.h"
#include "hardware/structs/usb.h"

#include "../include/code_config.h"
#include "../include/hw_sleep.h"
#include "../include/wrap_watchdog.h"


static bool     hw_pll_usb_on = true;
static bool     hw_ad_on;
static UsbMode  hw_usb_mode;
static uint16_t hw_sleep_minutes;
static uint64_t t_end;
static uint64_t t_now;
static uint32_t t_now_h = 0;
static uint32_t t_now_l;
static uint32_t t_now_l_old = 0;



/**
 * @brief Converte o timerawl que é 32 bits em t_now que é 64 bits
 *        - cada vez que timerawl for menor que o timerawl anterior quer dizer que houve o overflow do contador
 *        assim o t_now_h deverá ser incrementado de 1
 */
static void get_t_now(){
    t_now_l = timer_hw->timerawl;
    if(t_now_l < t_now_l_old) t_now_h++;
    t_now_l_old = t_now_l;
    t_now = t_now_h;
    t_now = t_now << 32;
    t_now = t_now | t_now_l;
}

static void hw_set_speed_full(){
    if(!HW_SLEEP_LOW_POWER) return;
    pll_init(
        pll_sys,
        1,          // refdiv
        1500 * MHZ, // VCO = 1500 MHz
        6,          // postdiv1
        2           // postdiv2
    );
    clock_configure(
        clk_sys,
        CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
        CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
        125000000,
        125000000
    );

    // liga se o usm mode não for USB_MODE_OFF ou se 
    if(((hw_usb_mode != USB_MODE_OFF)||(hw_ad_on))&&(!hw_pll_usb_on)){
        pll_init(       // Liga o PLL
            pll_usb,
            1,          // refdiv (12 MHz / 1)
            480 * MHZ,  // VCO = 480 MHz
            5,          // postdiv1
            2           // postdiv2
        );
        hw_pll_usb_on = true;
    }
    switch(hw_usb_mode){
        case USB_MODE_OFF:  // não liga
        case USB_MODE_ON:   // já está ligada
            break;
        case USB_MODE_OFF_ON:   // Religa a USB
            clock_configure(clk_usb,    // seta o clock para o do PLL
                0,                  // src (não usado)
                CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                48000000,
                48000000
            );
            stdio_usb_init();   // Reinicializa stdio USB

            // Força reconexão
            usb_hw->sie_ctrl &= ~USB_SIE_CTRL_PULLUP_EN_BITS;
            sleep_ms(50);   // Original era 10
            usb_hw->sie_ctrl |= USB_SIE_CTRL_PULLUP_EN_BITS;
            for(int i=0;i<USB_TURN_ON_TIME_S;i++){
                wrap_watchdog_update();
                sleep_ms(1000); // original não tinha
            }
            wrap_watchdog_update();
    }
}

/**
 * @brief Reduz o clock sys e desativa plls
 * 
 */
static void hw_set_speed_low(){
    if(!HW_SLEEP_LOW_POWER) return;
    switch(hw_usb_mode){
        case USB_MODE_OFF:  // não desliga pois já esta desliga
        case USB_MODE_ON:   // não desliga
            break;
        case USB_MODE_OFF_ON:   // Desliga USB
            stdio_usb_deinit();
            clock_stop(clk_usb);
            //pll_deinit(pll_usb);
            break;
    }
    if((hw_usb_mode != USB_MODE_ON)&&(hw_pll_usb_on)){  // Só não desliga o pll usb se USB_MODE_ON
        pll_deinit(pll_usb);
        hw_pll_usb_on = false;
    }

    clock_configure(
        clk_sys,
        CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF,
        0,
        12000000,
        12000000
    );

    // faltou desligar o pll_sys
    pll_deinit(pll_sys);
}


void hw_sleep_init(UsbMode usb_mode, bool ad_on, uint16_t sleep_minutes){
    hw_usb_mode      = usb_mode;
    hw_ad_on         = ad_on;
    hw_sleep_minutes = sleep_minutes;

    t_now = timer_hw->timerawl;
    t_end = t_now;
    if(hw_sleep_minutes) t_end += 60000000 * hw_sleep_minutes;
                    else t_end += 10000000;

    if(!HW_SLEEP_LOW_POWER) return;

    // Desativa clocks dos gpios
    clock_stop(clk_gpout0);
    clock_stop(clk_gpout1);
    clock_stop(clk_gpout2);
    clock_stop(clk_gpout3);

    switch(hw_usb_mode){
        case USB_MODE_OFF:
            printf("Disable USB forever\n");
            sleep_ms(200);
            stdio_usb_deinit();
            clock_stop(clk_usb);
            //pll_deinit(pll_usb); // será desligado no próximo sleep
            break;
        case USB_MODE_ON:       // não desliga a USB
        case USB_MODE_OFF_ON:   // a USB só é desligada durante o sleep
            break;
    }

    //clock_stop(clk_peri);
    //lock_stop(clk_rtc);
    //rosc_disable();
    //rosc_hw->ctrl = ROSC_CTRL_ENABLE_VALUE_DISABLE;

    sleep_ms(100);
}

void hw_sleep(){
    bool st = true;

    sleep_ms(100);

    #ifdef SLEEP_STATE_BY_GPIO
        gpio_put(SLEEP_STATE_GPIO_0, true);
    #endif
        
    hw_set_speed_low();

    #ifdef SLEEP_STATE_BY_GPIO
        gpio_put(SLEEP_STATE_GPIO_1, st);
    #endif
    
    get_t_now();
    while(t_end > t_now){
        wrap_watchdog_update();
        get_t_now();
        #ifdef SLEEP_STATE_BY_GPIO
            gpio_put(SLEEP_STATE_GPIO_1, st);
            st = !st;
        #endif
    }
    #ifdef SLEEP_STATE_BY_GPIO
        gpio_put(SLEEP_STATE_GPIO_1, false);
    #endif

    hw_set_speed_full();


    if(hw_sleep_minutes) t_end += 60000000 * hw_sleep_minutes;
                    else t_end += 10000000;

    #ifdef SLEEP_STATE_BY_GPIO
        gpio_put(SLEEP_STATE_GPIO_0, false);
    #endif
}