#include <Arduino.h>
#include "config.h"
#include "lorawan.h"
#include "uart_com.h"

typedef enum{
    MAIN_STATE_SLEEP = 0,
    MAIN_STATE_SLEEP_ERROR,
    MAIN_STATE_SLEEP_TIMEOUT,
    MAIN_STATE_WAIT_CMD,
    MAIN_STATE_SENDING_MSG
} MainState;

static MainState main_state;

void setup() {
    Serial.begin(115200);
    uart_setup();    
    lorawam_setup();
    delay(10000);
    Serial.println(SETUP_MSG);
    SerialUART.println(SW_BUILD);
    SerialUART.println("Monitor On");

    // configura para sair do sleep pelo RX_PIN
    gpio_wakeup_enable(RX_PIN, GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();

    main_state = MAIN_STATE_SLEEP;
    Serial.println("Ready System!");
}

void loop() {
    lorawam_update_state();

    switch(main_state){
        case MAIN_STATE_SLEEP:
            Serial.println("Sleeping");
            delay(50);
            esp_light_sleep_start();    // Entrando em light sleep até GPIO20 acordar
            // Acordou do sleep
            // delay(50);
            Serial.println("WakeUp");
            uart_start_read_data();
            main_state = MAIN_STATE_WAIT_CMD;
            break;
        case MAIN_STATE_SLEEP_ERROR:
            SerialUART.print('E');
            main_state = MAIN_STATE_SLEEP;
            break;
        case MAIN_STATE_SLEEP_TIMEOUT:
            SerialUART.print('T');
            main_state = MAIN_STATE_SLEEP;
            break;
        case MAIN_STATE_WAIT_CMD:
            switch(uart_state()){
                case UART_ST_WAITING_DATA:
                    break;
                case UART_ST_DATA_READY:
                    main_state = MAIN_STATE_SENDING_MSG;
                    break;
                case UART_ST_DATA_ERROR:
                    main_state = MAIN_STATE_SLEEP;
                    break;
                case UART_ST_TIMEOUT:
                    main_state = MAIN_STATE_SLEEP;
                    break;
            }
            break;
        case MAIN_STATE_SENDING_MSG:
            if(lorawan_state() == LORAWAN_STATE_SLEEPING){  // Já enviou a msg, pode dormir
                SerialUART.print('O');
                main_state = MAIN_STATE_SLEEP;
            }
            break;
    }
}