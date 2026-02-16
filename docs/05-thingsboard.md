# 📊 ThingsBoard CE — Processamento, Dashboards e Análise (TTN → TB)

Este documento descreve o uso do **ThingsBoard Community Edition (CE)** como servidor local (Raspberry Pi) para:
- ingestão via **TTN Webhook**,
- **processamento** na Rule Engine (Rule Chain),
- visualização em **dashboards**,
- e **análise exploratória** das variáveis ambientais e do enlace LoRaWAN.

> Instalação detalhada via Docker não é repetida aqui: ver referência oficial ao final.

---

## 1) Papel do ThingsBoard na arquitetura

Fluxo atual:

End Device → Gateway RAK7289CV2 → TTN → Webhook HTTP → ThingsBoard → Dashboards / Export CSV / GitLab


- O **TTN** realiza recepção/organização e decodificação do payload (formatter).
- O **ThingsBoard** recebe JSON via webhook, normaliza e persiste telemetria em séries temporais.
- O **dashboard** é a camada de visualização + validação operacional.

Referências internas do repositório:
- Gateway (hardware e rede): `docs/03-gateway-rak7289.md`
- Plataforma TTN (config): `docs/04-ttn.md`

---

## 2) Rule Chain (ThingsBoard) — o que faz e onde está o código

A Rule Chain implementada tem como objetivo converter o JSON da TTN em telemetria consistente por device, sem quebrar widgets.

Figura (visão estrutural da Rule Chain):

![Rule Chain](docs/assets/Rulechain.png)

### Script principal (sem colar aqui)

O código do node Script (“TTN router”) está versionado separadamente:

- `rulechains/ttn_router.js`

Esse script é responsável por:
- extrair `decoded_payload`
- normalizar números (`null` quando inválido)
- validar `latitude/longitude` (evita erro no mapa)
- extrair **RSSI/SNR** (`rx_metadata`)
- extrair parâmetros LoRa (**SF/BW/CR**) (`settings.data_rate.lora`)
- extrair `fcnt`
- calcular métricas derivadas:
  - `dew_point`, `dew_point_depression`, `condensation_risk`
  - `vpd`, `vpd_category`
- definir `metadata.deviceName = device_id` para originator correto

---

## 3) Telemetria e métricas derivadas (o que significa)

Esta seção descreve **as métricas** que passam a existir no ThingsBoard após o processamento do Rule Chain.

### 3.1 Variáveis ambientais (base)

- `temperature` (°C)
- `humidity` (%)
- `pressure` (hPa)
- `luminosity` (lux)
- `Vsys` (mV)
- `altitude` (m)
- `latitude`, `longitude`

Referência de chaves e unidades:

![Referência telemetria e unidades](docs/assets/Referencia_telemetria_unidades.png)

---

### 3.2 Qualidade do enlace LoRaWAN

- `rssi` (dBm)
- `snr` (dB)
- `fcnt`
- `lora_sf`
- `lora_bandwidth`
- `lora_cr`

Esses parâmetros permitem avaliar estabilidade, margem de ruído e comportamento do enlace.

Exemplo visual:

![RSSI e SNR](docs/assets/RSSI_SNR.png)

**Interpretação (exploratória):**
- RSSI mais negativo → sinal mais fraco
- SNR positivo → boa separação do ruído
- Variações ao longo do tempo podem estar associadas a:
  - posição/obstrução
  - interferência
  - mudanças no ambiente
  - condições de propagação

---

### 3.3 Dew Point (Ponto de orvalho) e risco de condensação

Campos:
- `dew_point` (°C)
- `dew_point_depression` (°C) = `T - Td`
- `condensation_risk` = `ALTO / MEDIO / BAIXO`

**Uso:**
- Identificar condições com maior chance de condensação (importante para eletrônica em campo e caixas fechadas).
- `dew_point_depression` pequeno indica proximidade de saturação.

Exemplo visual (relacionado a temperatura/umidade):

![Temperatura e umidade](docs/assets/Temp_humedad.png)

---

### 3.4 VPD (Vapor Pressure Deficit)

![VPD](docs/assets/VPD.png)

Campos:
- `vpd` (kPa)
- `vpd_category` (Muito Úmido / Baixo / Ótimo / Alto / Muito Alto)

**Uso:**
- Indicador do “potencial evaporativo” do ar.
- Aplicável a análise microclimática e cenários ambientais/agro.

---

## 4) Dashboards — por que cada imagem existe (e o que analisar)

### 4.1 Dashboard principal (visão geral)

![Dashboard principal](docs/assets/Dashboard_principal.png)

**Função:**
- validar que os devices aparecem como entidades,
- confirmar ingestão contínua,
- oferecer visão rápida de status e navegação.

---

### 4.2 Tabela de séries temporais (sanidade e depuração)

![Tabela de séries temporais](docs/assets/Time_series_table_data.png)

**O que analisar:**
- frequência real de amostragem
- campos que aparecem/desaparecem (dependência do payload)
- coerência básica (faixas plausíveis)

---

### 4.3 Energia: Vsys vs Luminosidade

![Vsys e luminosidade](docs/assets/Vsys_luminosidade.png)

**O que analisar:**
- correlação com ciclo dia/noite (carga/descarga)
- quedas abruptas (sintoma de falha de alimentação ou bateria)

---

### 4.4 Temperatura ambiente vs temperatura do microcontrolador

![Temp ambiente vs micro](docs/assets/Temp_amb_micro.png)

**O que analisar:**
- diferença persistente indica aquecimento interno/encapsulamento
- ajuda a justificar limitações de medição sem abrigo meteorológico

---

### 4.5 Mapa (geovisualização)

![Mapa](docs/assets/Mapa.png)

**O que analisar:**
- consistência de coordenadas
- confirmação de localização
- “saltos” de posição indicam GPS instável ou dado inválido (mitigado via validação no Rule Chain)

---

## 5) Limitações e validade dos dados (fase atual)

Os dados ainda são de caráter **experimental**, pois:
- posicionamento e instalação dos sensores não seguem padrão normativo completo
- não há abrigo meteorológico certificado
- ambiente urbano influencia propagação e medição

A visualização e análise devem ser interpretadas como:
> validação de infraestrutura + análise exploratória  
> e não como estação meteorológica certificada.

---

## 6) Referências

- ThingsBoard (Docker install): https://thingsboard.io/docs/user-guide/install/docker/
- ThingsBoard Docs: https://thingsboard.io/docs/
- TTN Docs: https://www.thethingsnetwork.org/docs/
- WMO – Guide to Instruments and Methods of Observation (2018)
