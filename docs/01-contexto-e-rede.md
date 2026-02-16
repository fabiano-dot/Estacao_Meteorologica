# 🌐 Contexto e Infraestrutura de Rede

Esta seção descreve o ambiente de rede no qual a infraestrutura LoRaWAN está inserida. A compreensão deste contexto é fundamental para a replicação do sistema, para a análise de suas limitações operacionais e para o planejamento de evoluções futuras da arquitetura.

---

## 🏫 Ambiente institucional

A infraestrutura LoRaWAN encontra-se implantada no ambiente da **Universidade Estadual de Campinas (UNICAMP)**, utilizando um segmento de rede institucional destinado a laboratórios e projetos acadêmicos.

O sistema está inserido em um range institucional específico (antigo DSIF), caracterizado por:

- Segmentação de rede (VLAN dedicada)
- Atribuição manual de endereços IP (sem DHCP)
- Políticas de segurança centralizadas
- Controle de portas e protocolos por padrão
- Monitoramento e gerenciamento pela equipe de TI

A configuração manual exige definição explícita de:

- Endereço IP
- Máscara de rede
- Gateway padrão
- Servidor DNS

Essas características impactam diretamente a arquitetura adotada para a infraestrutura LoRaWAN.

---

## 🔌 Conectividade do Gateway LoRaWAN

O gateway LoRaWAN está conectado à rede institucional por meio de **interface cabeada (Ethernet)**, sendo esta a única interface utilizada para o encaminhamento dos pacotes LoRaWAN ao servidor de rede.

Principais características da configuração:

- Endereço IP fixo configurado manualmente
- Inserção em segmento de rede autorizado
- Dependência de liberação de portas para tráfego externo
- Utilização de protocolo UDP pelo *packet forwarder*

O acesso Wi-Fi do gateway é utilizado exclusivamente para fins administrativos locais e não participa do encaminhamento de dados LoRaWAN.

---

## 📡 Comunicação LoRaWAN sob políticas institucionais

A comunicação entre o gateway e o servidor de rede LoRaWAN ocorre por meio de um *packet forwarder*, que utiliza o protocolo **UDP** para envio dos pacotes recebidos via rádio.

Em ambientes institucionais, o tráfego UDP pode não ser permitido por padrão. Para viabilizar a integração com servidores externos (como a The Things Network – TTN), foi necessário:

- Solicitar liberação explícita das portas UDP correspondentes
- Garantir que o gateway estivesse configurado corretamente no segmento DSIF
- Validar conectividade após alterações de ponto físico ou VLAN

Essas condições demonstram a dependência da infraestrutura LoRaWAN em relação às políticas de segurança da instituição.

---

## 🖥️ Servidor local (Raspberry Pi 5)

O servidor local, baseado em **Raspberry Pi 5**, está configurado no mesmo segmento institucional com endereço IP fixo manualmente atribuído.

Esse servidor hospeda serviços de backend e visualização, como:

- ThingsBoard CE
- Serviços auxiliares de exportação e armazenamento de dados

Aspectos relevantes:

- A comunicação interna entre gateway e servidor depende da segmentação de rede
- Alterações de infraestrutura podem exigir reconfiguração manual
- A ausência de DHCP implica controle total sobre a configuração IP

A arquitetura atual permite tanto integração com servidor externo (TTN) quanto potencial operação totalmente interna via servidor LoRaWAN autogerenciado.

---

## 📌 Considerações Técnicas

O contexto de rede institucional constitui um dos principais fatores condicionantes da infraestrutura LoRaWAN documentada neste projeto.

As decisões arquiteturais adotadas foram influenciadas diretamente por:

- Segmentação de rede (VLAN institucional)
- Ausência de DHCP no segmento utilizado
- Necessidade de configuração manual de IP
- Liberação controlada de portas UDP
- Políticas de segurança da universidade

A correta compreensão desse ambiente é essencial para:

- Replicação da infraestrutura em contexto semelhante
- Planejamento de migração para servidor LoRaWAN próprio (ex.: ChirpStack)
- Avaliação de independência em relação a serviços externos
