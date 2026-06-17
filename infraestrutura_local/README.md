# Infraestrutura Local

Esta pasta contém evidências de operação e monitoramento da infraestrutura local de coleta, processamento e armazenamento de dados meteorológicos.

---

## Visão geral

A infraestrutura é executada em uma **Raspberry Pi 5 (8 GB RAM, 256 GB SSD NVMe)** e opera de forma totalmente local, sem dependência de serviços externos em nuvem. Os seguintes serviços são executados em contêineres Docker:

| Serviço | Função |
|---|---|
| ChirpStack v4 | Servidor de rede LoRaWAN — gerenciamento de dispositivos e processamento de uplinks |
| Mosquitto | Broker MQTT — integração ChirpStack → ThingsBoard |
| ThingsBoard CE 4.3.0 | Armazenamento, visualização e exportação dos dados |
| PostgreSQL | Banco de dados relacional (backend do ThingsBoard) |

O acesso remoto é disponibilizado via **Cloudflare Tunnel**, sem exposição direta de portas na rede institucional.

---

## Período de operação e uptime

| Métrica | Valor |
|---|---|
| Início da coleta de dados | 02/02/2026 |
| Uptime do servidor (última reinicialização) | 5 semanas, 5 dias |
| Uptime dos contêineres Docker | Up 5 weeks (todos os serviços) |

Medido via `uptime -p` e `docker ps --format "table {{.Names}}\t{{.Status}}"`:

```
up 5 weeks, 5 days, 23 hours, 45 minutes

chirpstack-docker-chirpstack-gateway-bridge-1   Up 5 weeks
chirpstack-docker-chirpstack-rest-api-1         Up 5 weeks
chirpstack-docker-chirpstack-1                  Up 5 weeks
chirpstack-docker-postgres-1                    Up 5 weeks
chirpstack-docker-redis-1                       Up 5 weeks
chirpstack-docker-mosquitto-1                   Up 5 weeks
tb-thingsboard-ce-1                             Up 5 weeks
tb-postgres-1                                   Up 5 weeks
```

Os dados históricos estão disponíveis publicamente no HuggingFace:
👉 https://huggingface.co/datasets/adr1t0s/estacao-meteorologica

---

## Recursos do servidor

Monitoramento realizado via painel sysadmin do ThingsBoard em 13/06/2026.

| Recurso | Valor |
|---|---|
| CPU | ~1% (4 cores) |
| RAM | ~46% de 8 GB |
| Disco | ~20% de 234 GB |

O consumo de recursos permaneceu estável ao longo do dia, sem picos significativos.

![Recursos do sistema](figures/thingsboard_recursos_sistema.png)
![Recursos — histórico 24h](figures/thingsboard_recursos_24h.png)

---

## Dispositivos ativos e mensagens transportadas

Período de referência: **11/06/2026 12:00 a 12/06/2026 00:00**

| Métrica | Valor |
|---|---|
| Dispositivos ativos | **4** (constante ao longo do período) |
| Transport messages por intervalo de 30 min | ~372 |
| Transport messages por hora | ~744 |

![Atividade — dispositivos 12h](figures/thingsboard_activity_devices_12h.png)
![Atividade — mensagens 12h](figures/thingsboard_activity_messages_12h.png)

---

## Mensagens processadas pela Rule Engine

Estatísticas do nó **Save Timeseries** da Rule Chain principal (ChirpS Root) no mesmo período.

| Métrica | Valor |
|---|---|
| Mensagens processadas por hora | ~182–187 |
| Esperado teórico (4 estações) | 186 |
| Erros ocorridos | **0** |

Os valores observados são consistentes com a configuração de transmissão das estações (em02, em06, em10 a 60 s e em04 a 600 s), confirmando que nenhuma mensagem foi descartada pela infraestrutura.

![Rule Engine Statistics](figures/thingsboard_rule_engine_statistics.png)

---

## Latência de processamento

A latência foi estimada com base em dois componentes mensuráveis:

**1. Airtime LoRaWAN** — calculado para um pacote real da em02 (20 bytes de payload, SF7, BW 125 kHz, CR 4/5):

| Parâmetro | Valor |
|---|---|
| Payload | 20 bytes |
| Spreading Factor | SF7 |
| Bandwidth | 125 kHz |
| Code Rate | CR 4/5 |
| Airtime | **~56.6 ms** |

**2. Processamento no servidor** — medido via Debug do nó Save Timeseries:

| Parâmetro | Valor |
|---|---|
| ChirpStack (gwTime → nsTime) | ~16 ms |
| Save Timeseries (IN → OUT) | 7–37 ms |

**Latência total estimada (airtime + servidor): ~80–110 ms**

> Nota: o timestamp `aq_timestamp_ms` gerado pela estação não foi utilizado para este cálculo por estar sujeito a drift do relógio interno do microcontrolador.

---

## Comportamento após reinicialização e perda de conexão

### Reinicialização do sistema

Medido via `systemd-analyze blame`:

```
11.160s docker.service
 5.995s NetworkManager-wait-online.service
 5.991s cloudflared.service
```

Os três serviços inicializam em paralelo. Como os contêineres Docker comunicam-se internamente e não dependem da rede externa para operar, o tempo dominante é **~11s** até os serviços locais estarem operacionais. O acesso remoto via Cloudflare Tunnel fica disponível em até **~6s adicionais**. Todos os serviços estão configurados com `restart: always` no Docker Compose.

### Perda temporária de conexão com a internet

Testado desligando e religando a interface de rede via `nmcli`:

```bash
start=$(date +%s%3N); sudo nmcli networking off; sleep 10; sudo nmcli networking on; \
while ! ping -c1 8.8.8.8 &>/dev/null; do sleep 1; done; \
end=$(date +%s%3N); echo "Recovery time: $((end-start)) ms"
# Recovery time: 15478 ms
```

O resultado de 15478 ms inclui 10s de espera forçada do script. O tempo real de recuperação de conectividade após religar a rede foi de **~5.5s**. Durante todo o período, a coleta e o armazenamento local permanecem ininterruptos — apenas o acesso externo via Cloudflare Tunnel é afetado temporariamente.
