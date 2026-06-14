# Comunicação LoRaWAN — Estações Meteorológicas FEEC/Unicamp

Esta pasta contém os dados de telemetria LoRaWAN coletados pelas estações meteorológicas instaladas no campus da FEEC/Unicamp, juntamente com informações sobre a configuração da rede e a localização dos dispositivos.

---

## Estações

A rede conta com **4 estações simultâneas** operando em modo ABP Classe A sobre LoRaWAN AU915 (sub-bandas 0 e 1, canais 0–15).

| Métrica | em02 | em04 | em06 | em10 |
|---|---|---|---|---|
| Latitude | -22.822205 | -22.822132 | -22.819542 | -22.818361 |
| Longitude | -47.065704 | -47.065639 | -47.071064 | -47.062000 |
| Distância ao gateway (m) | 35 | 45 | 606 | 605 |
| Canal | 8 | 8 | 6 | 6 |
| Frequência (MHz) | 916.8 | 916.8 | 916.4 | 916.4 |
| rfChain | 2 | 2 | 1 | 1 |
| Spreading Factor | SF7 | SF7 | SF7 | SF10 |
| Data Rate | DR5 | DR5 | DR5 | DR2 |
| TX Power (dBm) | 14 | 14 | 14 | 14 |
| Bandwidth (kHz) | 125 | 125 | 125 | 125 |
| Intervalo de transmissão (s) | 60 | 600 | 60 | 60 |
| Pacotes recebidos (24h) | 1440 | 143 | 1401 | 1439 |
| PDR (%) | 100.0 | 99.3 | 97.3 | 99.9 |
| RSSI médio (dBm) | -81.0 | -94.9 | -103.2 | -89.3 |
| RSSI mín / máx (dBm) | -88 / -78 | -106 / -90 | -113 / -87 | -98 / -82 |
| SNR médio (dB) | 13.2 | 11.7 | 1.3 | 10.1 |
| SNR mín / máx (dB) | 0.8 / 14.5 | 5.3 / 13.3 | -8.5 / 8.8 | -8.5 / 12.8 |
| GPS ativo | Sim | Sim | Não | Não |

> **em04:** o intervalo de 600 s foi configurado para gestão energética, uma vez que o módulo GPS apresenta consumo elevado, reduzindo a autonomia da bateria durante períodos sem geração solar.  
> **em06 / em10:** operam sem GPS, o que reduz o consumo e permite maior autonomia em campo.

---

## Gateway

| Parâmetro | Valor |
|---|---|
| Modelo | RAK7289CV2 |
| Localização | FEEC – Edifício H, Unicamp |
| Latitude | -22.82236 |
| Longitude | -47.06600 |
| Servidor de rede | ChirpStack v4 (Docker, Raspberry Pi 5) |

---

## Configuração LoRaWAN

| Parâmetro | Valor |
|---|---|
| Região | AU915 |
| Sub-bandas | 0 e 1 (canais 0–15) |
| Método de ativação | ABP (Activation By Personalization) |
| Classe | A |
| Code Rate | CR 4/5 |

---

## Arquivos de telemetria

Os arquivos CSV contêm os dados de telemetria brutos exportados via API REST do ThingsBoard. Cada arquivo corresponde a uma estação e a um período de 24 horas.

| Arquivo | Estação | Período |
|---|---|---|
| `telemetria_em02_2026-06-12_00h00_2026-06-13_00h00.csv` | em02 | 12/06/2026 |
| `telemetria_em04_2026-06-08_00h00_2026-06-09_00h00.csv` | em04 | 08/06/2026 |
| `telemetria_em06_2026-06-12_00h00_2026-06-13_00h00.csv` | em06 | 12/06/2026 |
| `telemetria_em10_2026-06-12_00h00_2026-06-13_00h00.csv` | em10 | 12/06/2026 |

### Colunas

| Coluna | Descrição | Unidade |
|---|---|---|
| `Data/Hora` | Timestamp de recepção no servidor | UTC |
| `RSSI (dBm)` | Received Signal Strength Indicator | dBm |
| `SNR (dB)` | Signal-to-Noise Ratio | dB |
| `Frame Counter` | Contador de quadros LoRaWAN (fCnt) | — |
| `Data Rate` | Data Rate LoRaWAN (DR0–DR5, AU915) | — |

> Registros duplicados (mesmo timestamp e fCnt) foram removidos, mantendo sempre o último registro recebido.

---

## Mapa de localização

![Localização das estações](figures/Pos_em_rak.png)

Localização do gateway RAK7289CV2 e das quatro estações meteorológicas no campus FEEC/Unicamp.
