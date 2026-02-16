# 📊 Apêndice – Análise Técnica dos Dados e Métricas Derivadas

Este documento apresenta uma análise técnica preliminar dos dados coletados pela infraestrutura LoRaWAN documentada neste projeto.

O objetivo deste apêndice é:

- Interpretar os dados exibidos nos dashboards
- Explicar as métricas derivadas implementadas na Rule Chain
- Discutir limitações experimentais
- Identificar possibilidades futuras de análise avançada

Este documento **não descreve configuração técnica**, apenas interpretação e análise.

---

# 🌡️ Variáveis Ambientais Monitoradas

A infraestrutura atualmente coleta e processa as seguintes variáveis:

- Temperatura do ar (`temperature`)
- Umidade relativa (`humidity`)
- Pressão atmosférica (`pressure`)
- Luminosidade (`luminosity`)
- Tensão do sistema (`Vsys`)
- Coordenadas GPS (`latitude`, `longitude`)
- Métricas LoRaWAN (`rssi`, `snr`, `fcnt`)
- Parâmetros físicos LoRa (`lora_sf`, `lora_bandwidth`, `lora_cr`)

Essas variáveis permitem análises ambientais e também avaliação da qualidade do enlace LoRaWAN.

---

# 💧 Métricas Psicrométricas Derivadas

Na Rule Chain do ThingsBoard foram implementadas métricas derivadas baseadas em temperatura e umidade.

## 🌫️ 1. Temperatura de Orvalho (Dew Point)

A temperatura de orvalho é calculada utilizando a aproximação de Magnus:

\[
\alpha = \frac{aT}{b+T} + \ln\left(\frac{RH}{100}\right)
\]

\[
T_d = \frac{b \alpha}{a - \alpha}
\]

Onde:
- T = temperatura (°C)
- RH = umidade relativa (%)
- a = 17.27
- b = 237.7

No sistema:
- `dew_point`
- `dew_point_depression` (T - Td)

### Interpretação

- Diferença < 2 °C → Alto risco de condensação
- Diferença entre 2–5 °C → Risco moderado
- Diferença > 5 °C → Baixo risco

Isso permite avaliar condições de formação de orvalho ou condensação em superfícies.

---

## 🌱 2. VPD – Déficit de Pressão de Vapor

O VPD é calculado como:

\[
SVP = 0.6108 \cdot e^{\left(\frac{17.27T}{T+237.3}\right)}
\]

\[
VPD = SVP - (SVP \cdot RH/100)
\]

No sistema:
- `vpd`
- `vpd_category`

### Classificação implementada

- < 0.4 kPa → Muito Úmido
- 0.4–0.8 kPa → Baixo
- 0.8–1.2 kPa → Ótimo
- 1.2–1.6 kPa → Alto
- > 1.6 kPa → Muito Alto

### Aplicação

O VPD é amplamente utilizado em:

- Agricultura
- Estufas
- Monitoramento microclimático
- Modelagem de evapotranspiração

---

# 📡 Análise de Enlace LoRaWAN

Os parâmetros analisados incluem:

- RSSI (Received Signal Strength Indicator)
- SNR (Signal-to-Noise Ratio)
- Spreading Factor (SF)
- Bandwidth (BW)
- Coding Rate (CR)
- Frame Counter (FCnt)

## 📶 RSSI

Valores típicos observados:
- -60 dBm a -90 dBm → excelente
- -90 dBm a -110 dBm → aceitável
- < -120 dBm → crítico

## 📡 SNR

- > 10 dB → excelente
- 0 a 10 dB → estável
- < 0 dB → limiar de recepção

Essas métricas permitem avaliar:

- Qualidade do posicionamento do gateway
- Estabilidade do enlace
- Potenciais interferências
- Necessidade de ajuste de SF

---

# 📉 Interpretação Experimental dos Dados

⚠️ Importante:

Os dados coletados ainda possuem caráter experimental.

Limitações atuais:

- Sensores sem abrigo meteorológico normatizado
- Altura de instalação não padronizada
- Influência de superfícies próximas (paredes, concreto)
- Ambiente urbano com possíveis reflexões e interferência RF

Referência técnica:

WMO – Guide to Instruments and Methods of Observation (2018)

Portanto, os dashboards devem ser interpretados como:

> Validação da infraestrutura técnica  
> e não como estação meteorológica certificada.

---

# 🔍 Correlações Observadas

Durante a análise preliminar, foram observadas:

- Relação direta entre temperatura e VPD
- Aumento de luminosidade associado a aumento de temperatura
- Estabilidade do enlace mesmo com variação ambiental moderada

Essas observações ainda são qualitativas e dependem de séries temporais mais longas.

---

# 🤖 Possibilidades Futuras com IA

A estrutura atual permite:

- Exportação estruturada para CSV
- Processamento com Python (Pandas, NumPy)
- Aplicação de modelos estatísticos
- Treinamento de modelos de Machine Learning

Possíveis aplicações:

- Detecção de anomalias ambientais
- Identificação automática de falhas de sensor
- Modelagem preditiva de temperatura
- Classificação de qualidade de enlace LoRa
- Análise multivariada (Temperatura × Umidade × Luminosidade × VPD)

Essas análises encontram-se em fase de planejamento e não fazem parte do baseline atual.

---

# 📌 Conclusão do Apêndice

A infraestrutura implementada demonstra:

- Capacidade de coleta contínua
- Processamento derivado em tempo real
- Estrutura adequada para análise avançada
- Pipeline robusto de exportação e versionamento

O sistema encontra-se tecnicamente validado para:

- Monitoramento experimental
- Estudos de enlace LoRaWAN
- Desenvolvimento de análises ambientais futuras

A evolução para análises mais rigorosas dependerá de:

- Padronização de instalação
- Aumento do período de coleta
- Calibração formal dos sensores
