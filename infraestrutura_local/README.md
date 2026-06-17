
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

## Período de operação

A coleta de dados está em operação desde **02/02/2026**. Os dados históricos estão disponíveis publicamente no HuggingFace:
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

## Comportamento após reinicialização e perda de conexão

### Reinicialização do sistema

Medido via `systemd-analyze blame`:

```
11.160s docker.service
 5.995s NetworkManager-wait-online.service
 5.991s cloudflared.service
```

Os três serviços inicializam em paralelo. Como os contêineres Docker (ChirpStack, Mosquitto, ThingsBoard, PostgreSQL) comunicam-se internamente e não dependem da rede externa para operar, o tempo dominante é **~11s** até os serviços locais estarem operacionais. O acesso remoto via Cloudflare Tunnel fica disponível em até **~6s adicionais**. Todos os serviços estão configurados com `restart: always` no Docker Compose.

### Perda temporária de conexão com a internet

Testado desligando e religando a interface de rede via `nmcli`:

```bash
start=$(date +%s%3N); sudo nmcli networking off; sleep 10; sudo nmcli networking on; \
while ! ping -c1 8.8.8.8 &>/dev/null; do sleep 1; done; \
end=$(date +%s%3N); echo "Recovery time: $((end-start)) ms"
# Recovery time: 15478 ms
```

O resultado de 15478 ms inclui 10s de espera forçada do script. O tempo real de recuperação de conectividade após religar a rede foi de **~5.5s**. Durante todo o período, a coleta e o armazenamento local permanecem ininterruptos — apenas o acesso externo via Cloudflare Tunnel é afetado temporariamente.

---

## Pendente

- **Uptime do sistema** — `uptime -p`
- **Uptime dos contêineres Docker** — `docker ps --format "table {{.Names}}\t{{.Status}}"`
- **Latência entre transmissão e exibição**
