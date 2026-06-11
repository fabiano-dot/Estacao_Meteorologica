/**
 * @file    aq_data_ad.c
 * @author  your name (you@domain.com)
 * @brief   Driver do AD para leitura da MPU Temperature e VSys
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "hardware/adc.h"
#include "hardware/clocks.h" 
#include "../include/aq_data_ad.h"
#include "../include/loop_printf.h"


#define ADC_VSYS_CHANNEL_NUM    2
#define ADC_VSYS_GPIO           28

#define R1      100
#define R2      100
#define KRV     (3.3/4095*(R1+R2)/R2) * 50      // *50 ==> 20mV por step

static uint16_t aqdataad_vsys_k_10000;

int aqdataad_init_power_on(bool vsys, bool temp, uint16_t vsys_k_10000){
    aqdataad_vsys_k_10000 = vsys_k_10000;
    adc_set_temp_sensor_enabled(false);
    if(vsys) adc_gpio_init(ADC_VSYS_GPIO);
    adc_run(false);
    clock_stop(clk_adc);

    return 0;
}
int aqdataad_init_aq(bool vsys, bool temp){
    clock_configure(
        clk_adc,
        0, // sem glitchless mux
        CLOCKS_CLK_ADC_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
        48000000, // frequência da fonte (PLL USB)
        48000000  // frequência final do ADC
    );
    adc_init();

    if(temp) adc_set_temp_sensor_enabled(true);
    return 0;
}

int aqdataad_read_temp(int16_t *value){
    adc_select_input(ADC_TEMPERATURE_CHANNEL_NUM);
    sleep_us(10);
    uint16_t ad  = adc_read();

    // t   =  27   -    (ADC_Voltage - 0.706)/0.001721
    *value = (27.0 - (((3.3*ad/4095) - 0.706)/0.001721)) * 10.0;
    if(DEBUG_ON_AD){
        loop_printf("- MPU Tempetature    = %3.1f Celsius\n", *value * 0.1);   
    }
    return 0;
}
int aqdataad_read_vsys(uint8_t *value){
    adc_select_input(ADC_VSYS_CHANNEL_NUM);
    sleep_us(10);

    uint16_t ad =  adc_read();

    //vsys = (3.3 * ad / 4095) * Kr = KRV * ad
    ad = KRV * (uint64_t)ad * aqdataad_vsys_k_10000 / 10000;
    if(ad > 0xFE) *value = 0xFE;
             else *value = ad;

    if(DEBUG_ON_AD){
        loop_printf("- VSys               = %d mV\n", *value * 20);   
    }
    return 0;
}

int aqdataad_sleep(bool vsys, bool temp){
    if(temp) adc_set_temp_sensor_enabled(false);
    adc_run(false);
    clock_stop(clk_adc);
    return 0;
}