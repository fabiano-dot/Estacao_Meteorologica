/**
 * @file    aq_data_bmep280.h
 * @author  Antonio-Carlos-Ricardo
 * @brief   Driver do sensor BME280 ou BMP280
 * @version 0.1
 * @date    2026-02-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "hardware/i2c.h"
#include "../include/aq_data_bmep280.h"
#include "../include/code_config.h"
#include "../include/loop_printf.h"


// ********** Parâmetros do CI **********
#define AQDATABMEP280_TIMEOUT_START_US    2000    // valor normal é de 450us
#define AQDATABMEP280_TIMEOUT_CONV_US   200000    // valor normal é 100 ms

#define AQDATABMEP280_CHIP_ID_BMP180    0x55
#define AQDATABMEP280_CHIP_ID_BMP280    0x58
#define AQDATABMEP280_CHIP_ID_BME280    0x60
#define AQDATABMEP280_CHIP_ID_BME680    0x61

#define AQDATABMEP280_REG_RESET         0xE0
#define AQDATABMEP280_REG_STATUS        0xF3
#define AQDATABMEP280_REG_CHIP_ID       0xD0
#define AQDATABMEP280_REG_CALIBRATION_1 0x88    
#define AQDATABMEP280_REG_CALIBRATION_2 0xE1    
#define AQDATABMEP280_REG_CTRL_HUM      0xF2
#define AQDATABMEP280_REG_CTRL_MEAS     0xF4

#define AQDATABMEP280_HUMIDITY_16_OVERSAMPLING      0x05
#define AQDATABMEP280_CTRL_TEMP_16_OVERSAMPLING     0xA0           // 16 oversamplis, 0x05 << 5
#define AQDATABMEP280_CTRL_PRESS_16_OVERSAMPLING    0x14           // 16 oversamplis, 0x05 << 2
#define AQDATABMEP280_CTRL_MODE                     0x01           // Forced Mode, faz a aquisição e aṕos entra em sleep-mode

#define AQDATABMEP280_CTRL (AQDATABMEP280_CTRL_TEMP_16_OVERSAMPLING | AQDATABMEP280_CTRL_PRESS_16_OVERSAMPLING | AQDATABMEP280_CTRL_MODE)
                            // 0xB5

/**
 * @brief Estrutura dos dados de calibração
 * 
 */
typedef struct {        // pag24
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

    // Only BME280
    uint8_t  not_used;
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
} AqDataBmep280_Calibration;

/**
 * @brief Estrutura dos dados RAW
 * 
 */
typedef struct {
    uint32_t temp;
    uint32_t press;
    uint16_t hum;       // Only BME280
} AqDataBmep280_RawValues;


static AqDataBmep280_Device      device_id  = AQDATABMEP280_DEV_ERROR;
static AqDataBmep280_Calibration calibration;
static AqDataBmep280_RawValues   raw_values;
static uint64_t                  t_init_aq;





// ********** Rotinas para comunicação básica com o CI **********
static uint8_t device_i2c_address = 0x76;   // ou 0x77

static int bmp280_write_byte(uint8_t start_register, uint8_t data){
    uint8_t d[] = {start_register, data};

    if (i2c_write_timeout_us(I2C_MAIN_BUS, device_i2c_address, d, 2, false, I2C_TIMEOUT_US_BMEP280) != 2) return -1;
    return 0;
}

static int registers_read(uint8_t start_register, uint8_t *data, uint8_t data_size){
    // Escreve o endereço do start_register
    if (i2c_write_timeout_us(I2C_MAIN_BUS, device_i2c_address, &start_register, 1, true, I2C_TIMEOUT_US_BMEP280) != 1) return -1;
    
    // Lê os dados
    if(i2c_read_timeout_us(I2C_MAIN_BUS, device_i2c_address, data, data_size, false, I2C_TIMEOUT_US_BMEP280) != data_size) return -2;

    return 0;
}


// ********** Rotinas para leituras do CI **********

/**
 * @brief Lê qual CI está conectado.
 *        O resultado é armazenado na variável local: device_id
 * 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
static int aqdatabmep280_read_device(){
    uint8_t data;

    device_id = AQDATABMEP280_DEV_ERROR;
    int ret = registers_read(AQDATABMEP280_REG_CHIP_ID, &data, 1);

    if(DEBUG_ON_BMEP280){
        loop_printf("- BMEP280            : read_device(): AD=0x%02x, data=0x%02x, ret=%d  %s\n",
            device_i2c_address,
            data,
            ret, 
            data==AQDATABMEP280_CHIP_ID_BMP280?
                "bmP280":
                (data==AQDATABMEP280_CHIP_ID_BME280?
                    "bmE280":
                    "ERROR"));
    }

    if(ret) return ret;
    switch(data){
        case AQDATABMEP280_CHIP_ID_BMP280: device_id = AQDATABMEP280_DEV_BMP280; break;
        case AQDATABMEP280_CHIP_ID_BME280: device_id = AQDATABMEP280_DEV_BME280; break;
        default: return -1000;
    }
    return 0;
}

/**
 * @brief Lê os parâmetros de calibração do CI
 *        O resultado é armazenado na variável local: calibration
 * 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro
 */
static int aqdatabmep280_read_calibration(){
    uint8_t n_itens;

    switch(device_id){
        case AQDATABMEP280_DEV_BMP280: n_itens = 24; break;
        case AQDATABMEP280_DEV_BME280: n_itens = 26; break;
        default: return -1000;
    }

    // Lê dados de calibraçã, AQDATABMEP280_REG_CALIBRATION_1 até 26 bytes
    int ret = registers_read(AQDATABMEP280_REG_CALIBRATION_1, (uint8_t *)&calibration, n_itens);
    if(ret) return -1;

    if(DEBUG_ON_BMEP280){
        loop_printf("- BMEP280            : Calibration data1 : ");
        for(int i=0;i<n_itens;i++) loop_printf("%02x ", (&calibration)[i]);
        loop_printf("\n");
    }

    // Só le os demais parâmetros se BME280
    if(device_id != AQDATABMEP280_DEV_BME280) return 0;

    uint8_t data[7];
    // AQDATABMEP280_REG_CALIBRATION_2 até 16 bytes
    int ret2 = registers_read(AQDATABMEP280_REG_CALIBRATION_2, data, 7);
    if(ret2) return -2;

    calibration.dig_H2 =  (data[0]&0x00FF)       | ((data[1]&0x00FF)<<8);
    calibration.dig_H3 =   data[2];
    calibration.dig_H4 =  (data[4]&0x000F)       | ((data[3]&0x00FF)<<4);
    calibration.dig_H5 = ((data[4]&0x00F0) >> 4) | ((data[5]&0x00FF)<<4);
    calibration.dig_H6 =   data[6];

    if(DEBUG_ON_BMEP280){
        loop_printf("- BMEP280            : Calibration data2 : ");
        for(int i=0;i<7;i++) loop_printf("%02x ", data[i]);
        loop_printf("\n");
    }

    return 0;
}

/**
 * @brief Lê os dados RAW da aquisição
 *        - Aguarda o fim da aquisição
 *        - Lê os dados RAW da aquisição e armazena na variável local: raw_values
 * 
 * @return int Código de erro:
 *         - 0: leitura realizada com sucesso
 *         - diferente de 0: código indicando tipo de erro 
 */
static int aqdatabmep280_read_raw_values(){
    uint8_t size;
    uint8_t data[8];
    int ret = 0;

    raw_values.hum   = 0;
    raw_values.press = 0;
    raw_values.temp  = 0;

    switch(device_id){
        case AQDATABMEP280_DEV_BMP280: size = 6; break;
        case AQDATABMEP280_DEV_BME280: size = 8; break;
        default: return -1000;
    }

    // aguarda fim da conversão com timeout para caso de falha
    uint64_t t0 = time_us_64();
    bool fim = false;
    while(!fim){
        ret = registers_read(AQDATABMEP280_REG_STATUS, data, 1);
        if(!(data[0] & 0x08)) fim = true;
        else if(time_us_64() > (t_init_aq + AQDATABMEP280_TIMEOUT_CONV_US)){
            ret = -100;
        }
    }
    uint64_t t1 = time_us_64();

    if(DEBUG_ON_BMEP280){
        loop_printf("- BME280 DT From init= %d us\n", (uint32_t)(t0 - t_init_aq));
        loop_printf("- BME280 DT wait     = %d us\n", (uint32_t)(time_us_64() - t0));
        loop_printf("- BME280 DT Convert  = %d us\n", (uint32_t)(time_us_64() - t_init_aq));     
    }
    if(ret) return ret;


    ret = registers_read(0xF7, data, 8);
    if(ret) return ret;

    raw_values.press = data[0];
    raw_values.press = raw_values.press << 8;
    raw_values.press |= data[1];
    raw_values.press = raw_values.press << 4;
    raw_values.press |= data[2] >> 4;

    raw_values.temp = data[3];
    raw_values.temp = raw_values.temp << 8;
    raw_values.temp |= data[4];
    raw_values.temp = raw_values.temp << 4;
    raw_values.temp |= data[5] >> 4;

    if(device_id == AQDATABMEP280_DEV_BME280){
        raw_values.hum = data[6];
        raw_values.hum = raw_values.hum << 8;
        raw_values.hum |= data[7];
    }

    if(DEBUG_ON_BMEP280){
        loop_printf("- BME280 Raw data    : ");
        for(int i=0;i<size;i++) loop_printf("%02x ", data[i]);
        loop_printf("\n");
        loop_printf("- BME280 Temperature = Raw 0x%08x\n", raw_values.temp);
        loop_printf("- BME280 Pressure    = Raw 0x%08x\n", raw_values.press);
        loop_printf("- BME280 Humidity    = Raw 0x%04x\n", raw_values.hum);        
    }

    return 0;
};


// ********** Rotinas de Compensação **********

// Definições usadas na rotina
#define BME280_S32_t         int32_t
#define BME280_U32_t        uint32_t
#define BME280_S64_t         int64_t


// ********** From BME280 datasheet pag. 25 **********

static BME280_S32_t t_fine;     // t_fine carries fine temperature as global value

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T)
{
    BME280_S32_t var1, var2, T;
    var1 = ((((adc_T>>3) - ((BME280_S32_t)calibration.dig_T1<<1))) * ((BME280_S32_t)calibration.dig_T2)) >> 11;
    var2 = (((((adc_T>>4) - ((BME280_S32_t)calibration.dig_T1)) * ((adc_T>>4) - ((BME280_S32_t)calibration.dig_T1))) >> 12) * ((BME280_S32_t)calibration.dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}
// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P)
{
    BME280_S64_t var1, var2, p;
    var1 = ((BME280_S64_t)t_fine) - 128000;
    var2 = var1 * var1 * (BME280_S64_t)calibration.dig_P6;
    var2 = var2 + ((var1*(BME280_S64_t)calibration.dig_P5)<<17);
    var2 = var2 + (((BME280_S64_t)calibration.dig_P4)<<35);
    var1 = ((var1 * var1 * (BME280_S64_t)calibration.dig_P3)>>8) + ((var1 * (BME280_S64_t)calibration.dig_P2)<<12);
    var1 = (((((BME280_S64_t)1)<<47)+var1))*((BME280_S64_t)calibration.dig_P1)>>33;
    if (var1 == 0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((BME280_S64_t)calibration.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((BME280_S64_t)calibration.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)calibration.dig_P7)<<4);
    return (BME280_U32_t)p;
}
// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
BME280_U32_t bme280_compensate_H_int32(BME280_S32_t adc_H)
{
    BME280_S32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)calibration.dig_H4) << 20) - (((BME280_S32_t)calibration.dig_H5) *
        v_x1_u32r)) + ((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r *
        ((BME280_S32_t)calibration.dig_H6)) >> 10) * (((v_x1_u32r * ((BME280_S32_t)calibration.dig_H3)) >> 11) +
        ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) * ((BME280_S32_t)calibration.dig_H2) +
        8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
        ((BME280_S32_t)calibration.dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (BME280_U32_t)(v_x1_u32r>>12);
}


// ********** Rotinas publicas **********

static bool init_error;

int aqdatabmep280_init_power_on(){
    device_i2c_address = BMEP280_ADDRESS;
    if(aqdatabmep280_read_device()) return 1;

    return aqdatabmep280_read_calibration();
}


int aqdatabmep280_init_aq(){
    int ret = 0;

    if(device_id == AQDATABMEP280_DEV_BME280){
        // Configura a leitura de umidade
        ret =  bmp280_write_byte(AQDATABMEP280_REG_CTRL_HUM, AQDATABMEP280_HUMIDITY_16_OVERSAMPLING);
    }

    // inicia conversão:
    ret +=  bmp280_write_byte(AQDATABMEP280_REG_CTRL_MEAS, AQDATABMEP280_CTRL) * 10;
    t_init_aq = time_us_64();
    if(ret){
        init_error = true;
        return ret;
    }

    // Aguarda o bit de aquisição em curso ser setado
    // Obs.: O bit de aquisição em curso demora em torno de 450 us por ser setado
    uint8_t data[1];
    bool fim = false;
    init_error = false;
    while(!fim){
        ret = registers_read(AQDATABMEP280_REG_STATUS, data, 1);
        if(data[0] & 0x08) fim = true;
        else if(time_us_64() > (t_init_aq + AQDATABMEP280_TIMEOUT_START_US)){
            // TimeOut
            init_error = true;
            return -100;
        }
    }

    if(DEBUG_ON_BMEP280){
        loop_printf("- BME280 DT Start    = %d us\n", (uint32_t)(time_us_64() - t_init_aq));     
    }

    return init_error?100:0;
}

int aqdatabmep280_read_i(AqDataBmep280_Value_I *value){
    value->humidity = 0xFF;
    value->press    = 0xFFFF;
    value->temp     = 0x7FFF;

    if(init_error) return -100;

    int ret = aqdatabmep280_read_raw_values();
    if(ret) return ret;

    int32_t  t = BME280_compensate_T_int32(raw_values.temp);     // 1/100 Celsisus
    uint32_t p = BME280_compensate_P_int64( raw_values.press);   // 1/256 hPa

    uint32_t u = 0xFF;

    if(device_id == AQDATABMEP280_DEV_BME280){
        u = bme280_compensate_H_int32( raw_values.hum);     // 1/1024 %
        u = u / 512;
        if(u>200) u = 200;
    }

    if(DEBUG_ON_BMEP280){
        loop_printf("- BME280 Temperature = %.2f Celsius\n", t * 0.01);
        loop_printf("- BME280 Pressure    = %.2f hPa\n",     p / 256.0 / 100.0);
        loop_printf("- BME280 Humidity    = %.1f %%\n",      u * 0.5);        
    }

    value->humidity = u;     // resolução 0,5%
    value->temp     = t;
    //value->press    = 60000 - (p * 50 / 256 / 100);    //resolução 0,02 hpa
    //value->press    = 60000 - (p / 512);    //resolução 0,02 hpa
    value->press    = 60000 - (p >> 9);    //resolução 0,02 hpa

    return 0;
}

int aqdatabmep280_sleep(){
    return 0;
}




// ********** Rotinas de Teste e comparação **********

// ********** Compensação com Double, From BME280 datasheet pag. 49 **********

// Returns temperature in DegC, double precision. Output value of “51.23” equals 51.23 DegC.
// t_fine carries fine temperature as global value


static double BME280_compensate_T_double(BME280_S32_t adc_T)
{
    double var1, var2, T;
    var1 = (((double)adc_T)/16384.0 - ((double)calibration.dig_T1)/1024.0) * ((double)calibration.dig_T2);
    var2 = ((((double)adc_T)/131072.0 - ((double)calibration.dig_T1)/8192.0) *
        (((double)adc_T)/131072.0 - ((double)calibration.dig_T1)/8192.0)) * ((double)calibration.dig_T3);
    t_fine = (BME280_S32_t)(var1 + var2);
    T = (var1 + var2) / 5120.0;
    return T;
}

// Returns pressure in Pa as double. Output value of “96386.2” equals 96386.2 Pa = 963.862 hPa
static double BME280_compensate_P_double(BME280_S32_t adc_P)
{
    double var1, var2, p;
    var1 = ((double)t_fine/2.0) - 64000.0;
    var2 = var1 * var1 * ((double)calibration.dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)calibration.dig_P5) * 2.0;
    var2 = (var2/4.0)+(((double)calibration.dig_P4) * 65536.0);
    var1 = (((double)calibration.dig_P3) * var1 * var1 / 524288.0 + ((double)calibration.dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0)*((double)calibration.dig_P1);
    if (var1 == 0.0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576.0 - (double)adc_P;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)calibration.dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)calibration.dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double)calibration.dig_P7)) / 16.0;
    return p;
}

// Returns humidity in %rH as as double. Output value of “46.332” represents 46.332 %rH
static double bme280_compensate_H_double(BME280_S32_t adc_H)
{
    double var_H;
    var_H = (((double)t_fine) - 76800.0);
    var_H = (adc_H - (((double)calibration.dig_H4) * 64.0 + ((double)calibration.dig_H5) / 16384.0 *
        var_H)) * (((double)calibration.dig_H2) / 65536.0 * (1.0 + ((double)calibration.dig_H6) /
        67108864.0 * var_H *
        (1.0 + ((double)calibration.dig_H3) / 67108864.0 * var_H)));
    var_H = var_H * (1.0 - ((double)calibration.dig_H1) * var_H / 524288.0);
    if (var_H > 100.0)
        var_H = 100.0;
    else if (var_H < 0.0)
    var_H = 0.0;

    return var_H;
}

void aqdatabmep280_teste(){ 
    int count = 0;
    uint8_t data;
    int ret;

    double  humf;
    int32_t humi;
    double  pressf;
    int32_t pressi;
    double  tempf;
    int32_t tempi;

    while(1){
        printf("bmE 280\n");
        device_i2c_address = 0x76;
        aqdatabmep280_read_device();
        aqdatabmep280_read_calibration();
        aqdatabmep280_read_raw_values();


        tempf = BME280_compensate_T_double(raw_values.temp);
        tempi = BME280_compensate_T_int32(raw_values.temp);

        pressf = BME280_compensate_P_double(raw_values.press);
        pressi = BME280_compensate_P_int64( raw_values.press);

        humf = bme280_compensate_H_double(raw_values.hum);
        humi = bme280_compensate_H_int32( raw_values.hum);

        printf("Temp=%f  %d, Press==%f  %d, Hum=%f  %d\n", tempf, tempi, pressf, pressi, humf, humi);
        sleep_ms(200);

        printf("bmP 280\n");
        device_i2c_address = 0x77;
        aqdatabmep280_read_device();
        aqdatabmep280_read_calibration();
        aqdatabmep280_read_raw_values();

        tempf = BME280_compensate_T_double(raw_values.temp);
        tempi = BME280_compensate_T_int32( raw_values.temp);

        pressf = BME280_compensate_P_double(raw_values.press);
        pressi = BME280_compensate_P_int64( raw_values.press);

        humf = bme280_compensate_H_double(raw_values.hum);
        humi = bme280_compensate_H_int32( raw_values.hum);

        printf("Temp=%f  %d, Press==%f  %d\n", tempf, tempi, pressf, pressi);
        sleep_ms(200);

        printf("\n\n");
    }
}