# ☁️ The Things Network (TTN) – Configuração da Plataforma

Este documento descreve a configuração da plataforma **The Things Network (TTN)** utilizada na infraestrutura LoRaWAN implementada na UNICAMP, incluindo:

- Registro do gateway
- Criação de aplicações
- Cadastro de dispositivos
- Métodos de ativação
- Decodificação de payload
- Integrações externas

O foco é exclusivamente a **configuração lógica da plataforma TTN**, não o hardware do gateway nem o servidor local.

---

# 🌍 Plataforma TTN utilizada

- **Plataforma:** The Things Network – Community Edition  
- **Console:** https://console.cloud.thethings.network  
- **Cluster utilizado:** `nam1` (Américas)  

A edição comunitária foi escolhida por:

- Disponibilidade gratuita
- Facilidade de integração
- Suficiência para testes acadêmicos
- Compatibilidade com o gateway RAK7289CV2 operando em modo UDP

---

# 📡 Gateway no TTN (visão lógica)

O gateway físico **RAK7289CV2 (WisGate Edge Pro)** foi registrado na plataforma TTN.

Neste documento ele é tratado apenas do ponto de vista lógico.

## Identificação no TTN

- **Gateway ID:** `gateway-unicamp`
- **Gateway EUI:** `AC1F09FFFE0FBBDF`
- **Cluster:** `nam1`
- **Frequency Plan:** Australia 915–928 MHz (FSB 2)
- **Public status:** Enabled
- **Packet Broker forwarding:** Enabled
- **Modo de conexão:** Semtech UDP Packet Forwarder

> ⚠️ A configuração física, IP institucional e parâmetros Ethernet estão documentados em `docs/03-gateway-rak7289.md`.

---

# 📦 Estrutura de aplicações

No TTN, os dispositivos são organizados em **Applications**, que agrupam dispositivos com o mesmo propósito lógico.

Cada aplicação contém:

- Identificadores dos dispositivos
- Método de ativação (ABP ou OTAA)
- Payload formatter
- Integrações (Webhooks)

Estrutura:

TTN Console
    
    Applications
    
    <application-name>
     
    End Devices
    
    Payload Formatter
    
    Integrations

---

# 🔐 Métodos de ativação utilizados

Durante a implementação foram utilizados dois métodos LoRaWAN:

---

## 1️⃣ ABP – Activation By Personalization

Utilizado exclusivamente para testes iniciais.

### Quantidade
- 10 dispositivos

### Motivo da escolha
- Validação rápida de uplink
- Não depende do procedimento de join
- Simplificação durante testes de conectividade

### Parâmetros configurados
- DevEUI
- DevAddr
- AppSKey
- NwkSKey

> ⚠️ As chaves criptográficas não são versionadas neste repositório.

### Observação técnica
O ABP é adequado para testes controlados, porém não é recomendado para ambientes de produção devido a limitações de segurança e gerenciamento.

---

## 2️⃣ OTAA – Over-The-Air Activation

Criado como conjunto paralelo de dispositivos.

### Quantidade
- 10 dispositivos

### Objetivos
- Testar o procedimento de join
- Avaliar segurança
- Simular cenário mais próximo de produção

### Parâmetros configurados
- DevEUI
- JoinEUI (AppEUI)
- AppKey

📌 OTAA é o método recomendado para ambientes produtivos.

---

# 📊 Monitoramento no TTN

Os dispositivos podem ser monitorados em:

TTN Console → Applications → <application> → End Devices → Live data


Informações observáveis:

- Join (OTAA)
- Uplink messages
- Frame Counter (FCnt)
- FPort
- Data Rate (SF / BW)
- RSSI
- SNR
- Gateway responsável (`gateway-unicamp`)
- Decoded payload

---

# 🧾 Payload Formatter

O TTN permite execução de código JavaScript para decodificação do payload binário.

## Características do decoder utilizado

- Implementado em JavaScript
- Executado no uplink
- Baseado em um **Byte de Controle (BC)**
- Define dinamicamente os campos presentes no payload
- Permite otimização do tamanho do pacote transmitido

O código encontra-se versionado em:

payload/ttn-uplink-decoder.js


Qualquer alteração no firmware do dispositivo deve ser refletida também no decoder.

---

# 🔗 Integrações com serviços externos

O TTN permite integração via Webhooks HTTP.

No contexto atual:

- Os dados decodificados são enviados via HTTP
- A autenticação utiliza API Keys do TTN
- Os dados são encaminhados para uma plataforma de visualização (ThingsBoard CE)

Fluxo simplificado:

    End Device
        ↓
    Gateway RAK7289CV2
        ↓
    TTN (Application Server)
        ↓
    Webhook HTTP
        ↓
    Servidor Local (Raspberry Pi)
        ↓
    ThingsBoard

> ⚠️ As API Keys não são versionadas neste repositório.

---

# ⚠️ Considerações sobre Downlink

Embora o TTN ofereça:

- Downlink
- Buffer de mensagens
- Configuração de gateway delay

Na arquitetura atual:

- O gateway opera em modo UDP
- Não mantém canal persistente
- Downlinks não são transmitidos on-air
- A comunicação prática é predominantemente uplink

Impacto:

- Operação efetiva uplink-only
- Possível retransmissão de pacotes pelos dispositivos
- Avaliação futura do modo LoRa Basics™ Station

---

# 📌 Decisões técnicas relevantes

A escolha da TTN como servidor inicial foi influenciada por:

- Restrições da rede institucional
- Necessidade de validação rápida
- Disponibilidade gratuita
- Facilidade de integração com Webhooks
- Compatibilidade com o gateway em modo UDP

---

# 📌 Escopo deste documento

Este documento cobre:

- Configuração lógica da TTN
- Registro do gateway
- Aplicações
- Dispositivos
- Métodos ABP / OTAA
- Payload formatter
- Integrações

Não cobre:

- Configuração física do gateway
- Infraestrutura de rede institucional
- Configuração do servidor local
- Análise de dashboards

Esses tópicos são tratados em documentos específicos.

---

# 📚 Referências

- https://console.cloud.thethings.network
- https://www.thethingsnetwork.org/docs/
- https://docs.rakwireless.com/product-categories/wisgate/rak7289v2/

