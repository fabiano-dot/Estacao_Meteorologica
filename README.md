# 📡 Estações Meteorológicas LoRaWAN – UNICAMP

## 🧩 Descrição

Este repositório armazena os dados de uma rede de monitoramento ambiental baseada em **LoRaWAN**, implantada no campus da **Universidade Estadual de Campinas (UNICAMP)**. As estações de sensoriamento são identificadas de em01 a em10 e estão sendo gradualmente implantadas ao longo do campus. A rede conta com um gateway LoRaWAN e uma plataforma de servidor autogerenciada para recepção, processamento e visualização dos dados.

---

## 🏗️ Arquitetura do sistema

### Hardware das estações
- **Raspberry Pi Pico** – coleta de sensores e coordenação
- **ESP32-C3 XIAO + RFM95W** – transmissão LoRaWAN (stack LMIC)
- **Sensores**: BME280 (temperatura, umidade, pressão), luminosidade, GPS, tensão do sistema (Vsys)

### Rede LoRaWAN
- **Gateway**: RAK7289CV2
- **Região**: AU915, sub-bandas 0 e 1 (16 canais), ABP Classe A
- **Servidor de rede**: ChirpStack v4 (self-hosted, Docker)

### Servidor e visualização
- **Hardware**: Raspberry Pi 5 (8GB) + SSD NVMe 256GB (PCIe 3.0 x4)
- **Plataforma**: ThingsBoard CE 4.3.0
- **Acesso remoto**: Cloudflare Tunnel

---

## 📂 Estrutura de dados

Os dados são exportados automaticamente via cron diário e organizados por data:

```
data/
└── YYYY/
    └── MM/
        └── DD/
            ├── emXX.csv
            └── owm_unicamp.csv
```

### emXX.csv

Dados de cada estação ativa. Colunas:

- Timestamp
- Temperatura (°C)
- Umidade Relativa (%)
- Pressão Atmosférica (hPa)
- Tensão do Sistema (mV)
- Luminosidade (lux)
- Latitude (°)
- Longitude (°)
- Altitude (m)
- RSSI (dBm)
- SNR (dB)
- DR
- Frame Counter
- Timestamp de Aquisição (ms)

### owm_unicamp.csv

Dados meteorológicos de referência coletados via [OpenWeatherMap API](https://thingsboard.io/docs/recipes/fetch-weather-data/), consultada automaticamente a cada 10 minutos nas coordenadas da FEEC pelo ThingsBoard. Os dados são armazenados como telemetria em um Asset dedicado e exportados diariamente junto com os dados das estações.

Colunas:

- Timestamp
- Temperatura OWM (°C)
- Umidade OWM (%)
- Pressão OWM (hPa)
- Vento (m/s)
- Direção Vento (°)
- Nuvens (%)
- Chuva 1h (mm)
- Latitude (°)
- Longitude (°)

---

## 🚧 Estado do projeto

- ✅ Rede LoRaWAN operacional
- ✅ ChirpStack v4 self-hosted com ADR ativo
- ✅ ThingsBoard CE com dashboards públicos
- ✅ Export automático diário
- ✅ Integração OpenWeatherMap
- 🔄 Estações em04–em10: implantação prevista
- 🔄 Plugin ML para ADR: em desenvolvimento
- 🔄 Mapeamento de cobertura: em desenvolvimento

---

## 🔗 Links

- 📊 [Dashboard público](https://iot.bitdoglablsm.space/dashboard)
- 🖥️ [ThingsBoard CE](https://thingsboard.io/docs/installation/docker/)
- 📡 [ChirpStack v4](https://www.chirpstack.io/docs/getting-started/docker.html)
- 🌐 [Cloudflare Tunnel](https://developers.cloudflare.com/cloudflare-one/connections/connect-networks/)
