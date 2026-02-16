# 🧰 Hardware utilizado

Esta seção descreve os componentes físicos empregados na infraestrutura LoRaWAN documentada neste projeto. As informações aqui apresentadas têm como objetivo permitir a replicação da infraestrutura em ambiente institucional semelhante.

---

## 📡 Gateway LoRaWAN

- **Modelo:** RAK7289V2 (WisGate Edge)
- **Sistema operacional:** WisGateOS
- **Frequência de operação:** Banda ISM 915 MHz (região Brasil)
- **Conectividade de rede:**
  - Ethernet (uplink principal)
  - Wi-Fi (utilizado apenas para administração local)
- **Função principal:**
  - Recepção de pacotes LoRaWAN provenientes dos dispositivos finais
  - Encaminhamento dos pacotes para o servidor de rede LoRaWAN por meio de *packet forwarder* (protocolo UDP)

O gateway encontra-se configurado para operar de forma contínua, realizando o encaminhamento dos pacotes LoRaWAN recebidos via rádio para o servidor de rede configurado (TTN na arquitetura atual).

---

## 📶 Antena

- **Tipo:** Antena LoRa externa
- **Faixa de operação:** Compatível com 915 MHz
- **Instalação:** Conectada externamente ao gateway

A antena é responsável pela recepção e transmissão dos sinais LoRa na banda ISM utilizada no Brasil. Seu posicionamento físico influencia diretamente:

- Alcance de comunicação
- Estabilidade do RSSI
- Relação sinal-ruído (SNR)

Aspectos relacionados à otimização do posicionamento e calibração não fazem parte do escopo desta documentação.

---

## 🖥️ Servidor local

- **Plataforma:** Raspberry Pi 5
- **Memória RAM:** 8 GB
- **Armazenamento:** SSD 256 GB
- **Sistema operacional:** Raspberry Pi OS (Linux)
- **Endereçamento de rede:** IP fixo configurado manualmente no segmento institucional

- **Função principal:**
  - Execução dos serviços de backend
  - Hospedagem da plataforma de visualização (ThingsBoard CE)
  - Execução de scripts de exportação e versionamento de dados
  - Ambiente para futura implantação de servidor LoRaWAN autogerenciado

---

## 🔌 Infraestrutura de rede associada

- O gateway encontra-se conectado à rede institucional da UNICAMP por meio de conexão cabeada (Ethernet).
- O servidor local está configurado no mesmo segmento institucional com atribuição manual de IP.
- A comunicação entre os componentes depende das políticas de rede definidas pela infraestrutura institucional.

---

## 📌 Observações

- O Wi-Fi do gateway é utilizado exclusivamente para acesso à interface de administração.
- O funcionamento adequado do gateway depende da liberação das portas UDP necessárias para comunicação com o servidor LoRaWAN.
- Esta seção descreve apenas a infraestrutura física, não incluindo detalhes de firmware dos dispositivos finais.
