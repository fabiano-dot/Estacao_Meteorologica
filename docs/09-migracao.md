# 🔄 Migração Controlada – TTN → ChirpStack (Servidor Local)

Este documento descreve o cenário técnico real para teste de migração da arquitetura LoRaWAN baseada na **TTN** para um servidor próprio utilizando **ChirpStack**, considerando o ambiente de rede do laboratório.

⚠️ Trata-se de um **teste controlado**, não de migração definitiva.

---

# 🌐 Contexto de Rede Disponível

A infraestrutura do laboratório opera em um range de IP institucional:

- Sub-rede: `143.106.8.128/25`
- Configuração manual (sem DHCP)
- Endereçamento estático

Atualmente:

| Equipamento | IP |
|-------------|------|
| Gateway RAK | `143.106.8.214` |
| Raspberry Pi | `143.106.8.215` |

Ambos estão na mesma sub-rede, permitindo:

- Comunicação direta L2/L3
- Ausência de NAT intermediário
- Sem necessidade de Wi-Fi institucional
- Ambiente ideal para teste de servidor LoRaWAN local

Isso viabiliza teste real de ChirpStack como Network Server.

---

# 🎯 Objetivo do Teste

Validar a arquitetura:

```

End Device
↓
Gateway RAK (UDP 1700)
↓
ChirpStack (Raspberry Pi)
↓
ThingsBoard (integração local)

````

Sem dependência da TTN.

---

# ⚙️ Etapa 1 – Configuração da Raspberry Pi

Configurar IP estático:

```bash
sudo nano /etc/dhcpcd.conf
````

Adicionar:

```
interface eth0
static ip_address=143.106.8.215/25
static routers=143.106.8.129
static domain_name_servers=143.106.8.30 143.106.2.5
```

Reiniciar:

```bash
sudo reboot
```

Verificar:

```bash
ip addr show eth0
ping 143.106.8.214
```

Se houver resposta, a comunicação local está funcional.

---

# ⚙️ Etapa 2 – Reconfiguração do Gateway

No WisGateOS:

1. Manter IP estático `143.106.8.214`
2. Alterar modo LoRaWAN:

   * Servidor UDP → IP da Raspberry
   * Porta: `1700`

Servidor:

```
143.106.8.215
```

Protocolo:

```
Semtech UDP Packet Forwarder
```

⚠️ Pode ser necessário factory reset dependendo do firmware.

---

# 🧱 Arquitetura com ChirpStack

ChirpStack é composto por:

* Gateway Bridge
* Network Server
* Application Server
* PostgreSQL
* Redis
* MQTT Broker

Execução recomendada via Docker.

---

# 🔗 Comunicação ChirpStack → ThingsBoard

Diferente da arquitetura com TTN:

## ❌ Não será necessário Webhook HTTP

A integração pode ocorrer de duas formas:

### Opção 1 – MQTT (recomendado)

Fluxo:

```
ChirpStack → MQTT → ThingsBoard
```

O ChirpStack publica eventos em tópicos MQTT:

```
application/<appID>/device/<devEUI>/event/up
```

O ThingsBoard pode consumir via:

* MQTT Integration
* MQTT Transport

Vantagens:

* Comunicação local
* Baixa latência
* Sem dependência de Internet
* Arquitetura desacoplada

---

### Opção 2 – HTTP Integration (alternativa)

ChirpStack pode enviar dados via HTTP para ThingsBoard:

```
ChirpStack → HTTP POST → ThingsBoard
```

Mas isso replica o modelo TTN.

Para arquitetura local, MQTT é mais coerente.

---

# 📊 Comparação Arquitetural

| Aspecto                 | TTN            | ChirpStack Local |
| ----------------------- | -------------- | ---------------- |
| Dependência de Internet | Sim            | Não (opcional)   |
| Downlink funcional      | Limitado (UDP) | Completo         |
| Controle do NS          | Parcial        | Total            |
| Complexidade            | Baixa          | Média            |
| Observabilidade         | Limitada       | Alta             |

---

# 🧪 Testes Técnicos Recomendados

Após configuração:

## Verificar porta UDP 1700

Na Raspberry:

```bash
sudo netstat -ulnp | grep 1700
```

## Monitorar logs do ChirpStack

```bash
docker logs chirpstack -f
```

## Verificar recebimento de uplink

* Confirmar join (se OTAA)
* Confirmar recepção de uplink
* Confirmar publicação MQTT

---

# 📡 Comunicação Final Esperada

```
End Device
   ↓
Gateway (UDP 1700)
   ↓
ChirpStack
   ↓ MQTT
ThingsBoard
   ↓
Dashboards
```

Nesse modelo:

* Não há webhook externo
* Não há TTN intermediária
* Toda a arquitetura permanece on-premise

---

# ⚠️ Pontos Críticos

* Estabilidade da VLAN DSIF
* Sincronização de horário (NTP)
* Gerenciamento de múltiplos containers
* Crescimento do PostgreSQL

---

# 📌 Conclusão

O range DSIF permite teste real de servidor LoRaWAN próprio.

A migração deixa de ser apenas conceitual e passa a ser tecnicamente viável.

Entretanto:

* A TTN continua suficiente para uso predominantemente uplink.
* ChirpStack se justifica quando há necessidade de controle total ou operação offline.

---

