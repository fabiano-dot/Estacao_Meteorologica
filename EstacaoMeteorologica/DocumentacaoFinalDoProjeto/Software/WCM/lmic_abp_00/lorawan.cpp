#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "config.h"
#include "lorawan.h"


#define RFM95W_POWER    14


LorawanPars          lorawan_pars;
static LorawanState  lorawan_st;
static unsigned long last_do_send = 0;


/*
📋 Tabela-resumo — JOIN OTAA (AU915)
| Parâmetro | Valor                |
| --------- | -------------------- |
| Canais    | 64–71 (500 kHz)      |
| Bandwidth | 500 kHz              |
| DR        | DR6                  |
| SF        | SF8                  |
| CR        | 4/5                  |
| Header    | Explícito            |
| CRC       | ON                   |
| Potência  | Automática (~14 dBm) |
| ADR       | Não aplicável        |
*/



// Escolha se a mensagem será confirmada
bool useConfirmed = false;    // true = com ACK | false = sem ACK

// ===============================================================
// CHAVES OTAA  (preencher com seus valores)
// Obs.: APPEUI e DEVEUI = LSB FIRST
//       APPKEY = MSB FIRST
// ===============================================================
/*
static const u1_t PROGMEM APPEUI[8]  = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
static const u1_t PROGMEM DEVEUI[8]  = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
static const u1_t PROGMEM APPKEY[16] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

// Callbacks obrigatórios da LMIC para OTAA
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8); }
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8); }
void os_getDevKey (u1_t* buf) { memcpy_P(buf, APPKEY, 16); }
*/

// Callbacks obrigatórios da LMIC para OTAA
void os_getArtEui (u1_t* buf) { memcpy_P(buf, lorawan_pars.appeui, 8); }
void os_getDevEui (u1_t* buf) { memcpy_P(buf, lorawan_pars.deveui, 8); }
void os_getDevKey (u1_t* buf) { memcpy_P(buf, lorawan_pars.appkey, 16);}




// ===============================================================
// PINAGEM (ESP32-C3 XIAO + RFM95W) — AJUSTAR SE NECESSÁRIO
// ===============================================================

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 2,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 4,
    .dio = {3, 7, 6},
    .rxtx_rx_active = 0,
    .rssi_cal = 10,
    .spi_freq = 8000000,
    // Advanced configurations are passed to the pinmap via pConfig
    //.pConfig = &myConfig, //Talvez não precise
};


// ===============================================================
// VARIÁVEIS
// ===============================================================
static osjob_t sendjob;

static bool messagePending = false;
static uint8_t txBuffer[255];
static uint8_t txLength = 0;


// ===============================================================
// FUNÇÃO DE ENVIO
// ===============================================================
void do_send(osjob_t* j) {
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println("TX pendente, aguardando...");
        return;
    }

    //Serial.println("Before LMIC_setTxData2");
    LMIC_setTxData2(
        1,
        txBuffer,
        txLength,
        useConfirmed ? 1 : 0
    );

    Serial.print("Enviando ");
    Serial.print(txLength);
    Serial.print(" bytes...  dt=");
    Serial.print(millis() - last_do_send);
    Serial.println(" ms");
}


// ===============================================================
// CALLBACK DE EVENTOS LORAWAN
// ===============================================================
static bool joined = false;

void onEvent(ev_t ev) {
    switch (ev) {

    case EV_JOINING:
        Serial.println("EV_JOINING");
        break;

    case EV_JOINED:
        Serial.println("EV_JOINED SUCCESS");

        // Futuro: salvar sessão no NVS
        // TODO: persistir DevAddr, NwkSKey, AppSKey
        joined = true; 

        //LMIC_setAdrMode(0);
        //LMIC_setDrTxpow(DR_SF7, 14);
        //LMIC_disableChannel(64);
        LMIC_setLinkCheckMode(0);

        // Agora pode enviar
        do_send(&sendjob);
        break;

    case EV_JOIN_FAILED:
        Serial.println("EV_JOIN_FAILED");
        break;

    case EV_TXSTART:
        Serial.print("TX Start, DT = ");
        Serial.print(millis() - last_do_send);
        Serial.println(" ms");
        break;
    case EV_TXCOMPLETE:
        Serial.println("TX COMPLETE");

        // abp
        if (LMIC.txrxFlags & TXRX_ACK) {
            Serial.println("  - ACK RECEIVED");
            //Serial.write(0xAA); // aviso ACK na UART
        } else {
            Serial.println("  - No ACK Received");
            //Serial.write(0x55); // aviso TX sem ACK
        }
        Serial.print("  - DT = ");
        Serial.print(millis() - last_do_send);
        Serial.println(" ms");
        //Serial.println();

        messagePending = false;

        // Futuro: salvar frame counter
        // TODO: salvar LMIC.seqnoUp no NVS

        // Reduz consumo
        //esp_sleep_enable_timer_wakeup(10 * 1000000ULL);
        //esp_deep_sleep_start();

        lorawan_st = LORAWAN_STATE_SLEEPING;


        // otaa
        /*
        if (LMIC.dataLen) {
            Serial.print("RX ");
            Serial.print(LMIC.dataLen);
            Serial.println(" bytes");
        }*/

        break;
    default:
      Serial.print("Event: ");
      Serial.println(ev);
    }
}

void lorawam_setup(){
    SPI.begin(SPI_SCL, SPI_MISO, SPI_MOSI, SPI_CS);
    os_init();
}

void lorawam_update_state(){
    os_runloop_once();
}



void         lorawan_send(){
    uint32_t devaddr;
    u4_t     seqnoUp;
    dr_t     dr_sf;
    unsigned long now = millis();
    switch(lorawan_pars.mode){
        case LORAWAN_MODE_INVALID:
            break;
        case LORAWAN_MODE_LORA:
            break;
        case LORAWAN_MODE_ABP:
            lorawan_st = LORAWAN_STATE_BUSY;
            os_init();
            LMIC_reset();

            devaddr = (uint32_t)lorawan_pars.abp_devaddr[0];
            devaddr = (devaddr << 8) | (uint32_t)lorawan_pars.abp_devaddr[1];
            devaddr = (devaddr << 8) | (uint32_t)lorawan_pars.abp_devaddr[2];
            devaddr = (devaddr << 8) | (uint32_t)lorawan_pars.abp_devaddr[3];
            u1_t nwk[16];
            u1_t app[16];
            memcpy_P(nwk, lorawan_pars.nwkskey, 16);
            memcpy_P(app, lorawan_pars.appskey, 16);

            LMIC_setSession(0x1, devaddr, nwk, app);
            seqnoUp = lorawan_pars.abp_fcnt[0];
            seqnoUp = (seqnoUp << 8) | lorawan_pars.abp_fcnt[1];
            LMIC.seqnoUp = seqnoUp;     // valor salvo em NVS/EEPROM/Flash
            //LMIC.seqnoDn = 0;         // opcional

            LMIC_setAdrMode(0);
            

            switch(lorawan_pars.sf){
                case  7: dr_sf = DR_SF7;  break;
                case  8: dr_sf = DR_SF8;  break;
                case  9: dr_sf = DR_SF9;  break;
                case 10: dr_sf = DR_SF10; break;
                case 11: dr_sf = DR_SF11; break;
                case 12: dr_sf = DR_SF12; break;
                default: dr_sf = DR_SF7;
            }


            LMIC_setDrTxpow(dr_sf, RFM95W_POWER);

            // =======================================================
            // CANAIS AU915 CONFIGURADOS (BRASIL)
            // Desabilita tudo e habilita um sub-band (exemplo SB2)
            // =======================================================
            for (int i = 0; i < 72; i++) LMIC_disableChannel(i);

            if(lorawan_pars.channel <= 0x3F){
                LMIC_enableChannel(lorawan_pars.channel);
            }else {
                switch(lorawan_pars.channel){
                    case 0xFD: for (int i = 0; i <= 7;  i++) LMIC_enableChannel(i); break;
                    case 0xFE: for (int i = 8; i <= 15; i++) LMIC_enableChannel(i); break;
                    case 0xFF:
                    default:   for (int i = 0; i <= 15; i++) LMIC_enableChannel(i); break;
                }
            }
            
            LMIC_setLinkCheckMode(0);

            //if (!(LMIC.opmode & OP_TXRXPEND)) 

            txLength = lorawan_pars.msg_size;
            memcpy(txBuffer, lorawan_pars.msg, txLength);

            Serial.print("Enviando ABP MSG, Delta T last msg = ");
            Serial.print(now - last_do_send);
            Serial.println(" ms");

            last_do_send = now;
            Serial.println("do_send BEFORE");
            do_send(&sendjob);
            Serial.println("do_send AFTER");
            break;
        case LORAWAN_MODE_OTAA:
            /*  os_init()
                LMIC_reset()
                Configura AppEUI, DevEUI e AppKey
                Configura canais (AU915)
                Chama LMIC_startJoining()
                Aguarda evento EV_JOINED
                Só depois envia payload (LMIC_setTxData2())
            */
            lorawan_st = LORAWAN_STATE_BUSY;
            os_init();
            LMIC_reset();
            // Desativa ADR (opcional)
            LMIC_setAdrMode(0);

            // DataRate e potência
            LMIC_setDrTxpow(lorawan_pars.sf, 14);

            // ================================
            // AU915 – Brasil
            // ================================
            for (int i = 0; i < 72; i++) LMIC_disableChannel(i);

            if(lorawan_pars.channel <= 0x3F){
                LMIC_enableChannel(lorawan_pars.channel);
            }else {
                switch(lorawan_pars.channel){
                    case 0xFD: for (int i = 0; i <= 7;  i++) LMIC_enableChannel(i); break;
                    case 0xFE: for (int i = 8; i <= 15; i++) LMIC_enableChannel(i); break;
                    case 0xFF:
                    default:   for (int i = 0; i <= 15; i++) LMIC_enableChannel(i); break;
                }
            }

            LMIC_setLinkCheckMode(0);

            txLength = lorawan_pars.msg_size;
            memcpy(txBuffer, lorawan_pars.msg, txLength);

            // Inicia JOIN OTAA
            Serial.println("Iniciando OTAA Join...");
            LMIC_startJoining();


            break;
    }
}



LorawanState lorawan_state(){
  return lorawan_st;
}