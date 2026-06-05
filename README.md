# 📡 Estações Meteorológicas LoRaWAN – UNICAMP

## 🧩 Descrição

Este repositório armazena os dados de uma rede de monitoramento ambiental baseada em **LoRaWAN**, implantada no campus da **Universidade Estadual de Campinas (UNICAMP)**. O sistema é composto por até 10 estações de sensoriamento (em01–em10) distribuídas pelo campus, um gateway LoRaWAN e uma plataforma de servidor autogerenciada para recepção, processamento e visualização dos dados.

O projeto é desenvolvido no contexto acadêmico da FEEC/Unicamp, com caráter open-source e dashboards públicos.

---

## 🏗️ Arquitetura do sistema

### Hardware das estações
- **Raspberry Pi Pico** – coleta de sensores e coordenação
- **ESP32-C3 XIAO + RFM95W** – transmissão LoRaWAN (stack LMIC)
- **Sensores**: BME280 (temperatura, umidade, pressão), luminosidade, GPS, tensão do sistema (Vsys)

### Rede LoRaWAN
- **Gateway**: RAK7289CV2
- **Região**: AU915, sub-banda 0, ABP Classe A
- **Servidor de rede**: ChirpStack v4 (self-hosted, Docker)

### Servidor e visualização
- **Hardware**: Raspberry Pi 5 (8GB)
- **Plataforma**: ThingsBoard CE 4.3.0
- **Acesso remoto**: Cloudflare Tunnel
- **Dados climáticos**: integração OpenWeatherMap (polling a cada 10 min, coordenadas FEEC) — [referência](https://thingsboard.io/docs/recipes/fetch-weather-data/)

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

### Arquivos

- **emXX.csv** — dados de cada estação ativa. Colunas: timestamp, temperatura (°C), umidade relativa (%), pressão atmosférica (hPa), tensão do sistema (mV), luminosidade (lux), latitude, longitude, altitude (m), RSSI (dBm), SNR (dB), DR, frame counter, timestamp de aquisição (ms).

- **owm_unicamp.csv** — dados meteorológicos externos via OpenWeatherMap. Colunas: timestamp, temperatura (°C), umidade (%), pressão (hPa), velocidade do vento (m/s), direção do vento (°), cobertura de nuvens (%), precipitação 1h (mm), coordenadas.

---

## 🎯 Objetivos do projeto

- Monitoramento ambiental contínuo no campus da Unicamp
- Análise de desempenho de rede LoRaWAN (RSSI, SNR, PDR, ADR)
- Mapeamento de cobertura com dados GPS e modelos preditivos
- Geração de dataset aberto para pesquisa acadêmica

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
