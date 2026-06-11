/**
 * @file    aq_data.c
 * @author  Antonio-Carlos-Ricardo
 * @brief   Chama todos os drivers dos sensores
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "hardware/i2c.h"
#include "../include/aq_data.h"
#include "../include/code_config.h"


int aqdata_init_power_on( AqData *value, uint16_t lux_k_10000, uint16_t vsys_k_10000){
    i2c_init(         I2C_MAIN_BUS,      I2C_MAIN_BAUDRATE);
    gpio_pull_up(     I2C_MAIN_GPIO_SDA);
    gpio_pull_up(     I2C_MAIN_GPIO_SCL);
    gpio_set_function(I2C_MAIN_GPIO_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_MAIN_GPIO_SCL, GPIO_FUNC_I2C);

    if(value == NULL) return 0xFFFF;
    int ret = 0;
/*    if(value->active_sensors.battery){
        if(aqdatabat_init_power_on())            ret |= AQ_ITEM_BAT_VALUE;
    }*/
    if(value->active_sensors.bme280){
        if(aqdatabmep280_init_power_on())        ret |= AQ_ITEM_BME280;
    }
    if(value->active_sensors.gps){
        if(aqdatagps_init_power_on())            ret |= AQ_ITEM_GPS;
    }
    if(value->active_sensors.lux){
        if(aqdatalux_init_power_on(lux_k_10000)) ret |= AQ_ITEM_LUX;
    }
 
    if(aqdataad_init_power_on(value->active_sensors.vsys, value->active_sensors.cpu_temp, vsys_k_10000))
                                                 ret |= AQ_ITEM_CPU_TEMP | AQ_ITEM_VSYS;
    
    return ret;
}

int aqdata_init_aq( AqData *value){
    if(value == NULL) return 0xFFFF;
    int ret = 0;
/*    if(value->active_sensors.battery){
        if(aqdatabat_init_aq())     ret |= AQ_ITEM_BAT_VALUE;
    }*/
    if(value->active_sensors.bme280){
        if(aqdatabmep280_init_aq()) ret |= AQ_ITEM_BME280;
    }
    if(value->active_sensors.gps){
        if(aqdatagps_init_aq())     ret |= AQ_ITEM_GPS;
    }
    if(value->active_sensors.lux){
        if(aqdatalux_init_aq())     ret |= AQ_ITEM_LUX;
    }

    if(value->active_sensors.vsys || value->active_sensors.cpu_temp){
        if(aqdataad_init_aq(value->active_sensors.vsys, value->active_sensors.cpu_temp)) 
                                    ret |= AQ_ITEM_VSYS | AQ_ITEM_CPU_TEMP;
    }
    return ret;
}

int aqdata_read( AqData *value){
    if(value == NULL) return 0xFFFF;
    int ret = 0;
/*    if(value->active_sensors.battery){
        if(aqdatabat_read(&value->battery))           ret |= AQ_ITEM_BAT_VALUE;
    }*/
    if(value->active_sensors.bme280){;
        if(aqdatabmep280_read_i(&value->bmep280))     ret |= AQ_ITEM_BME280;
    }
    if(value->active_sensors.gps){
        if(aqdatagps_read(&value->gps))               ret |= AQ_ITEM_GPS;
    }
    if(value->active_sensors.lux){
        if(aqdatalux_read(&value->lux))               ret |= AQ_ITEM_LUX;
    }

    if(value->active_sensors.vsys){
        if(aqdataad_read_vsys(&value->vsys))          ret |= AQ_ITEM_VSYS;
    }
    if(value->active_sensors.cpu_temp){
        if(aqdataad_read_temp(&value->cpu_temp_deci)) ret |= AQ_ITEM_CPU_TEMP;
    }
    return ret;
}
int aqdata_sleep(AqData *value){
    if(value == NULL) return 0xFFFF;
    int ret = 0;
/*    if(value->active_sensors.battery){
        if(aqdatabat_sleep()) ret |= AQ_ITEM_BAT_VALUE;
    }*/
    if(value->active_sensors.bme280){
        //if(aqdatabme280_sleep()) ret |= AQ_ITEM_BME280;
        if(aqdatabmep280_sleep()) ret |= AQ_ITEM_BME280;
    }
    if(value->active_sensors.gps){
        if(aqdatagps_sleep()) ret |= AQ_ITEM_GPS;
    }
    if(value->active_sensors.lux){
        if(aqdatalux_sleep()) ret |= AQ_ITEM_LUX;
    }

    if(value->active_sensors.vsys || value->active_sensors.cpu_temp){
        if(aqdataad_sleep(value->active_sensors.vsys, value->active_sensors.cpu_temp)) ret |= AQ_ITEM_VSYS | AQ_ITEM_CPU_TEMP;
    }
    
    return ret;
}