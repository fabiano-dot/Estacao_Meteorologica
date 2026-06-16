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

## Ainda nao sei 

Indicadores que tal vez encuentre en el terminal da Rasp

- **Uptime do sistema** — `uptime -p`
- **Uptime dos contêineres Docker** — `docker ps --format "table {{.Names}}\t{{.Status}}"`
- **Comportamento após reinicialização** — os serviços são configurados com `restart: always` no Docker Compose, garantindo retomada automática; a ser confirmado com logs reais
- https://avbentem.github.io/airtime-calculator/ttn/au915/2,12

lsm@lsm:~ $ systemd-analyze blame
11.160s docker.service ---- contenedor 
 5.995s NetworkManager-wait-online.service ---- servicio de internet 
 5.991s cloudflared.service ---- tunnel como servicio "persistente"

 
 Docker + Cloudflare = 20s /// perdida de ene
 
 
 2.854s NetworkManager.service
 1.931s cloud-init-main.service
 1.918s cloud-config.service
 1.896s containerd.service servicio de cloudflaare para tunnel 
 1.803s ssh.service
 1.790s lightdm.service
 1.782s plymouth-quit-wait.service
 1.194s dev-sda2.device
 1.005s e2scrub_reap.service
  998ms udisks2.service
  952ms rpc-statd-notify.service
  884ms apt-daily.service
  857ms apt-daily-upgrade.service
  608ms ModemManager.service
  534ms rpi-eeprom-update.service
  504ms accounts-daemon.service
  467ms polkit.service
  440ms systemd-udevd.service
  388ms man-db.service
  378ms systemd-fsck@dev-disk-by\x2dpartuuid-eab2b7da\x2d01.service
lines 1-23...skipping...
11.160s docker.service ----- servicio de docker /// cotenedor de chirp 
 5.995s NetworkManager-wait-online.service
 5.991s cloudflared.service
 2.854s NetworkManager.service
 1.931s cloud-init-main.service
 1.918s cloud-config.service
 1.896s containerd.service
 1.803s ssh.service
 1.790s lightdm.service
 1.782s plymouth-quit-wait.service
 1.194s dev-sda2.device
 1.005s e2scrub_reap.service
  998ms udisks2.service
  952ms rpc-statd-notify.service
  884ms apt-daily.service
  857ms apt-daily-upgrade.service
  608ms ModemManager.service
  534ms rpi-eeprom-update.service
  504ms accounts-daemon.service
  467ms polkit.service
  440ms systemd-udevd.service
  388ms man-db.service
  378ms systemd-fsck@dev-disk-by\x2dpartuuid-eab2b7da\x2d01.service
  343ms rpi-setup-loop@var-swap.service
  302ms avahi-daemon.service
  296ms bluetooth.service
  286ms plymouth-start.service
  283ms dbus.service
  258ms rp1-test.service
  227ms user@1000.service
  220ms glamor-test.service
  215ms wpa_supplicant.service
  213ms logrotate.service
  199ms cloud-final.service
  195ms cloud-init-local.service
  169ms keyboard-setup.service
  164ms systemd-hostnamed.service
  161ms sshswitch.service

[1]+  Stopped                 systemd-analyze blame
lsm@lsm:~ $ 

 Caso solo caida de internet:
lsm@lsm:~ $ start=$(date +%s%3N); sudo nmcli networking off; sleep 10; sudo nmcli networking on; while ! ping -c1 8.8.8.8 &>/dev/null; do sleep 1; done; end=$(date +%s%3N); echo "Recovery time: $((end-start)) ms"
Recovery time: 15478 ms
lsm@lsm:~ $ 
