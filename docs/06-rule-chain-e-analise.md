# 🔄 Rule Chain e Modelo de Análise

Este documento descreve **exclusivamente** a lógica de processamento implementada na Rule Chain do ThingsBoard, bem como o modelo analítico aplicado sobre a telemetria recebida via TTN.

O foco aqui não é a infraestrutura da plataforma (descrita em `05-thingsboard.md`), mas sim:

- Processamento interno da telemetria
- Estrutura dos dados transformados
- Métricas derivadas calculadas
- Modelo lógico de análise ambiental e de enlace LoRaWAN

---

# 🧠 Papel da Rule Chain na Arquitetura

Fluxo lógico:

    TTN Webhook
    ↓
    ThingsBoard (HTTP Integration)
    ↓
    Root Rule Chain
    ↓
    Script Node (Processamento)
    ↓
    Save Timeseries
    ↓
    Dashboards


A Rule Chain é responsável por:

- Normalizar o JSON recebido
- Extrair `decoded_payload`
- Validar campos numéricos
- Validar latitude/longitude
- Associar `deviceName`
- Calcular métricas derivadas
- Persistir telemetria estruturada

---

# 📦 Estrutura de Entrada (TTN)

Campos principais recebidos:

- `uplink_message`
- `decoded_payload`
- `rx_metadata`
- `settings`
- `f_cnt`
- `end_device_ids.device_id`

---

# 🔎 Normalização e Validação

O script implementa:

### 1️⃣ Conversão numérica segura

- Strings inválidas → `null`
- `"error"`, `"nan"`, `"null"` → ignoradas
- Valores não finitos → descartados

### 2️⃣ Validação geográfica

Latitude válida:
- -90 ≤ lat ≤ 90

Longitude válida:
- -180 ≤ lon ≤ 180

Evita quebra do widget de mapa.

### 3️⃣ Extração de metadados LoRaWAN

De `rx_metadata`:

- `rssi`
- `snr`

De `settings.data_rate.lora`:

- `bandwidth`
- `spreading_factor`
- `coding_rate`

Também é extraído:

- `fcnt`

---

# 📊 Telemetria Persistida

As seguintes chaves são armazenadas como séries temporais:

## 🌡 Ambientais

- `temperature`
- `humidity`
- `pressure`
- `Vsys`
- `luminosity`
- `altitude`

## 📡 LoRaWAN

- `rssi`
- `snr`
- `lora_bandwidth`
- `lora_sf`
- `lora_cr`
- `fcnt`

## 📍 Geográficas

- `latitude`
- `longitude`

## 🧾 Controle

- `bc`
- `aq_timestamp_ms`
- `aq_timestamp_iso`
- `device_id`
- `device_name`

---

# 🧮 Métricas Derivadas Implementadas

A Rule Chain não apenas armazena dados brutos, mas também calcula métricas ambientais.

---

## 🌫 1️⃣ Dew Point (Temperatura de Orvalho)

Fórmula utilizada (Magnus-Tetens):

alpha = ((a * T) / (b + T)) + ln(RH / 100)
Td = (b * alpha) / (a - alpha)


Onde:

- T = temperatura (°C)
- RH = umidade relativa (%)
- a = 17.27
- b = 237.7

São calculados:

- `dew_point`
- `dew_point_depression`
- `condensation_risk`

Classificação:

| Diferença T - Td | Risco |
|------------------|--------|
| < 2 °C           | ALTO   |
| 2–5 °C           | MÉDIO  |
| > 5 °C           | BAIXO  |

---

## 💧 2️⃣ VPD (Vapor Pressure Deficit)

Cálculo:

SVP = 0.6108 * exp((17.27 * T) / (T + 237.3))
AVP = SVP * (RH / 100)
VPD = SVP - AVP


Unidade: kPa

Classificação adotada:

| VPD (kPa) | Categoria      |
|-----------|---------------|
| < 0.4     | Muito Úmido   |
| 0.4–0.8   | Baixo         |
| 0.8–1.2   | Ótimo         |
| 1.2–1.6   | Alto          |
| > 1.6     | Muito Alto    |

Essas métricas permitem:

- Avaliação de conforto térmico
- Monitoramento agrícola
- Estudo de microclima
- Detecção de risco de condensação

---

# 📈 Modelo Analítico Aplicado

A análise implementada pode ser dividida em três camadas:

---

## 🔹 Camada 1 — Ambiental

Correlação entre:

- Temperatura
- Umidade
- VPD
- Dew Point
- Luminosidade

Permite observar:

- Aquecimento diurno
- Resfriamento noturno
- Relação radiação × temperatura
- Saturação atmosférica

---

## 🔹 Camada 2 — Energia

Análise de:

- `Vsys`
- `luminosity`

Permite observar:

- Perfil de carga solar
- Descarga noturna
- Estabilidade da bateria
- Impacto do ciclo ambiental na energia

---

## 🔹 Camada 3 — Enlace LoRaWAN

Análise de:

- RSSI
- SNR
- SF
- Bandwidth
- FCnt

Permite avaliar:

- Estabilidade de transmissão
- Ruído ambiental
- Possível perda de pacotes
- Retransmissões

---

# 📌 Observações Técnicas

- A Rule Chain foi projetada para ser tolerante a falhas.
- Campos ausentes não interrompem o processamento.
- Valores inválidos são descartados.
- O sistema é compatível com expansão futura.

---

# 🚀 Possíveis Evoluções

- Detecção automática de anomalias
- Classificação climática automática
- Detecção de perda de enlace
- Modelos preditivos de bateria
- Machine Learning offline com CSV exportado

---

# 📚 Referências Técnicas

- Magnus-Tetens Approximation
- FAO Irrigation and Drainage Paper 56 (VPD)
- LoRaWAN Specification 1.0.3
- ThingsBoard Rule Engine Documentation
