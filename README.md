# 📡 Estações Meteorológicas LoRaWAN – UNICAMP

## 🧩 Descrição

Este repositório documenta e armazena os dados de uma rede de monitoramento ambiental baseada em **LoRaWAN**, implantada no campus da **Universidade Estadual de Campinas (UNICAMP)**. O sistema é composto por até 10 estações de sensoriamento (em01–em10) distribuídas pelo campus, um gateway LoRaWAN e uma plataforma de servidor autogerenciada para recepção, processamento e visualização dos dados.

O projeto é desenvolvido no contexto acadêmico da FEEC/Unicamp, com caráter open-source e dashboards públicos.

---

## 🏗️ Arquitetura do sistema

### Rede LoRaWAN
- **Gateway**: RAK7289CV2
- **Região**: AU915, sub-banda 0/1, ABP Classe A
- **Servidor de rede**: ChirpStack v4 (self-hosted, Docker)

### Servidor e visualização
- **Hardware**: Raspberry Pi 5 (8GB)
- **Plataforma**: ThingsBoard CE 4.3.0
- **Acesso público**: Cloudflare Tunnel
- **Dados climáticos**: Devices + integração OpenWeatherMap (polling a cada 10 min, coordenadas FEEC) ref: https://thingsboard.io/docs/recipes/fetch-weather-data/

---

## 📂 Estrutura de dados

Os dados são exportados automaticamente via cron diário e organizados por data:

data/
└── YYYY/
└── MM/
└── DD/
├── emXX.csv
├── owm_unicamp.csv
└── raw_json/

CSVs são gerados para as estações ativas. Cada arquivo contém: timestamp, temperatura, umidade, pressão, tensão do sistema, luminosidade, coordenadas GPS, RSSI, SNR, frame counter e timestamp de aquisição.

O arquivo `owm_unicamp.csv` contém dados meteorológicos externos (OpenWeatherMap): temperatura, umidade, pressão atmosférica, velocidade e direção do vento, cobertura de nuvens e precipitação.

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
- ✅ Export automático diário para GitHub
- ✅ Integração OpenWeatherMap
- 🔄 Estações em04–em10: implantação prevista
- 🔄 Plugin ML para ADR: em desenvolvimento
- 🔄 Mapeamento de cobertura: em desenvolvimento

---

## 🔗 Links

- 📊 [Dashboard público](https://iot.bitdoglablsm.space/dashboard)
- 🏫 FEEC/Unicamp
