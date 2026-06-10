# 📡 Estações Meteorológicas LoRaWAN – UNICAMP

Firmware e infraestrutura de servidor de uma rede de monitoramento ambiental baseada em **LoRaWAN**, implantada no campus da **Universidade Estadual de Campinas (UNICAMP)**. As estações são identificadas de `em01` a `em10` e estão sendo gradualmente implantadas ao longo do campus.

Os dados coletados estão disponíveis publicamente e são atualizados diariamente em: **[HuggingFace Dataset → adr1t0s/estacao-meteorologica](https://huggingface.co/datasets/adr1t0s/estacao-meteorologica)**

---

## 🏗️ Arquitetura do sistema

```
┌──────────────────────┐
│  Estação (em01–em10) │
│  RPi Pico + ESP32-C3 │
│  + RFM95W            │
└──────────┬───────────┘
           │ LoRaWAN AU915 (ABP Classe A)
           ▼
┌──────────────────────┐
│  Gateway RAK7289CV2  │
│  FEEC – Edifício H   │
└──────────┬───────────┘
           │ UDP Packet Forwarder
           ▼
┌──────────────────────┐     ┌──────────────────────┐
│  ChirpStack v4       │────▶│  ThingsBoard CE 4.3.0 │
│  (Docker, RPi 5)     │MQTT │  (Docker, RPi 5)      │
└──────────────────────┘     └──────────┬────────────┘
                                        │ REST API
                                        ▼
                              ┌──────────────────────┐
                              │  Export diário (cron) │
                              │  → HuggingFace        │
                              └──────────────────────┘
```

---

## 📡 Rede LoRaWAN

| Parâmetro | Valor |
|---|---|
| **Gateway** | RAK7289CV2 |
| **Região** | AU915 — sub-bandas 0 e 1 (16 canais) |
| **Modo** | ABP Classe A |
| **Servidor de rede** | ChirpStack v4 (self-hosted, Docker) |
| **ADR** | Ativado — algoritmo default + plugin ML em desenvolvimento |

---

## 🖥️ Servidor

| Componente | Descrição |
|---|---|
| **Hardware** | Raspberry Pi 5 (8 GB RAM) + SSD NVMe 256 GB |
| **OS** | Raspberry Pi OS (64-bit) |
| **ChirpStack v4** | Servidor de rede LoRaWAN (Docker) |
| **ThingsBoard CE 4.3.0** | Plataforma IoT — ingestão, regras e dashboards (Docker) |
| **Mosquitto** | MQTT broker |
| **Cloudflare Tunnel** | Acesso remoto seguro sem exposição de portas |
| **IP público** | `143.106.8.215` (rede institucional Unicamp) |

---

## 📂 Estrutura do repositório

```
Estacao_Meteorologica/
├── README.md
├── firmware/
│   ├── pico/               ← firmware Raspberry Pi Pico (C/C++)
│   └── esp32/              ← firmware ESP32-C3 XIAO (Arduino/LMIC)
│       ├── lorawan.cpp
│       ├── lorawan.h
│       └── lmic_config.h
└── .gitignore
```

Os scripts de exportação de dados e os CSVs **não estão neste repositório** — estão no [HuggingFace Dataset](https://huggingface.co/datasets/adr1t0s/estacao-meteorologica).

---

## ⚙️ Dependências do firmware (ESP32-C3)

| Biblioteca | Versão | Função |
|---|---|---|
| MCCI LMIC | v5.01 | Stack LoRaWAN |
| LoRa (Sandeep Mistry) | 0.8.0 | Driver RFM95W |
| Preferences | built-in | Persistência NVS (ADR state) |

**Importante:** `lmic_config.h` deve ter `#define CFG_au915` para operar na região AU915. O uso de `CFG_us915` resulta em incompatibilidade de canais.

---

## 🔑 ADR e NVS

O estado ADR (`datarate` e `txpow` negociados pelo ChirpStack) é persistido em NVS via `Preferences` para sobreviver aos ciclos de `LMIC_reset()` que ocorrem a cada transmissão ABP. Os valores são gravados no flash **apenas quando mudam**, para preservar os ciclos de escrita.

O menu serial da Pico permite configurar o SF como fixo (7–12) ou `AUTO` (0xFF), modo em que o ESP32 ignora o NVS e deixa o ChirpStack negociar via `LinkADRReq`.

---

## 🚧 Estado do projeto

| Componente | Status |
|---|---|
| Rede LoRaWAN operacional | ✅ |
| ADR com persistência NVS | ✅ |
| ChirpStack v4 self-hosted | ✅ |
| ThingsBoard CE com dashboards públicos | ✅ |
| Export automático diário para HuggingFace | ✅ |
| Plugin ML para ADR (ChirpStack JS + FastAPI) | 🔄 em desenvolvimento |
| Mapeamento preditivo de cobertura (Random Forest) | 🔄 em desenvolvimento |

---

## 🔗 Links

- 📦 [Dataset (HuggingFace)](https://huggingface.co/datasets/adr1t0s/estacao-meteorologica/tree/main)
- 📊 [Dashboard público](https://iot.bitdoglablsm.space/dashboard)
- 📡 [ChirpStack v4](https://www.chirpstack.io/docs/getting-started/docker.html)
- 🖥️ [ThingsBoard CE](https://thingsboard.io/docs/installation/docker/)
- 🌐 [Cloudflare Tunnel](https://developers.cloudflare.com/cloudflare-one/connections/connect-networks/)
