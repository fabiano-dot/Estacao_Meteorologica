#ifndef CONFIG_H
#define CONFIG_H

#define SW_BUILD   "0008"



#define ESP32C3_XIAO
//#define ESP32C3_SUPER_MINI_PLUS

#define UART_BUFFER_SIZE    256
#define UART_READ_T_MAX_MS  5000

// --- Conexão com o RFM95W ---
#ifdef ESP32C3_XIAO         // seed xiao
#define SPI_SCL     GPIO_NUM_8
#define SPI_MISO    GPIO_NUM_9
#define SPI_MOSI    GPIO_NUM_10
#define SPI_CS      GPIO_NUM_2
#define SPI_RESET   GPIO_NUM_4
#define PIN_DIO0    LMIC_UNUSED_PIN
#define PIN_DIO1    LMIC_UNUSED_PIN
#endif

#ifdef ESP32C3_SUPER_MINI_PLUS   // super mini plus
#define SPI_SCL     GPIO_NUM_4
#define SPI_MISO    GPIO_NUM_5
#define SPI_MOSI    GPIO_NUM_6
#define SPI_CS      GPIO_NUM_7
#define SPI_RESET   GPIO_NUM_10
#define PIN_DIO0    LMIC_UNUSED_PIN
#define PIN_DIO1    LMIC_UNUSED_PIN
#endif


#define RX_PIN GPIO_NUM_20
#define TX_PIN GPIO_NUM_21

#define CMD_LORA            L
#define CMD_LORAWAN_ABP     A
#define CMD_LORAWAN_OTAA    O

extern const char * SETUP_MSG;


#endif // CONFIG_H