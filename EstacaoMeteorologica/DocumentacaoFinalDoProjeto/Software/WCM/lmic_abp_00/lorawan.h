#ifndef LORAWAN_H
#define LORAWAN_H

typedef enum{
  LORAWAN_STATE_SLEEPING = 0,
  LORAWAN_STATE_BUSY
}LorawanState;

typedef enum{
  LORAWAN_MODE_INVALID = 0,
  LORAWAN_MODE_LORA,
  LORAWAN_MODE_ABP,
  LORAWAN_MODE_OTAA
}LorawanMode;

typedef struct{
  LorawanMode mode;
  uint8_t     channel;        // Lora, ABP, OTAA
  uint8_t     sf;             // Lora, ABP, OTAA
  uint8_t     abp_fcnt[2];    //       ABP
  uint8_t     abp_devaddr[4]; //       ABP
  uint8_t     appskey[16];    //       ABP
  uint8_t     nwkskey[16];    //       ABP
  uint8_t     deveui[8];      //            OTAA
  uint8_t     appeui[8];      //            OTAA
  uint8_t     appkey[16];     //            OTAA

  uint8_t     msg[UART_BUFFER_SIZE];  // Lora, ABP, OTAA
  int         msg_size;               // Lora, ABP, OTAA
}LorawanPars;

extern LorawanPars lorawan_pars;

void         lorawam_setup();
void         lorawam_update_state();
void         lorawan_send();
LorawanState lorawan_state();


#endif // LORAWAN_H