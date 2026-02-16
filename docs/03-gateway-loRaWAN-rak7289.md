# 📡 Gateway LoRaWAN – RAK7289CV2 (WisGate Edge)

Este documento describe la configuración, características técnicas y operación del **gateway LoRaWAN RAK7289CV2 (WisGate Edge Pro)** dentro de la infraestructura implementada en la **Universidade Estadual de Campinas (UNICAMP)**.

El foco está en el gateway como componente físico y lógico, su conectividad de red institucional y su integración con **The Things Network (TTN)**.

---

## 🧩 Visión general del equipo

- **Modelo:** RAK7289CV2 – WisGate Edge Pro  
- **Fabricante:** RAKwireless  
- **Versión:** 16 canales  
- **Tipo:** Gateway LoRaWAN Outdoor  
- **Sistema Operativo:** WisGateOS  
- **Backhaul:** Ethernet + LTE (soportado por hardware)  
- **Modo actual de operación:** Semtech UDP Packet Forwarder  
- **Frecuencia configurada:** AU915 (915–928 MHz)

### Rol en la arquitectura

El gateway tiene como función:

- Recibir paquetes LoRaWAN provenientes de dispositivos finales (end-devices)
- Encaminarlos al servidor de red LoRaWAN
- Mantener sincronización con el servidor externo

En la configuración actual, el servidor utilizado es:

> **The Things Network (TTN) – Cluster nam1**

---

## 📊 Capacidad teórica del gateway

Según especificaciones del fabricante:

- **16 canales LoRa**
- Soporte para aproximadamente **2.000 end-devices**
- Considerando bajo tráfico (ejemplo: 1 uplink cada 5 minutos)

Esta capacidad depende de:

- Duty cycle
- Spreading Factor
- Plan de frecuencias
- Nivel de congestión del espectro

---

# 📡 Antena utilizada

El gateway opera con una antena externa instalada separadamente.

## Especificaciones de la antena

- **Modelo:** 8 dBi Fiber Glass Antenna  
- **Rango de frecuencia:** 900–930 MHz  
- **Ganancia:** 8 dBi  
- **Conector:** N-Type a RP-SMA  
- **Tipo:** Outdoor (fiberglass)  
- **Compatibilidad:** Banda AU915  

La antena es un componente crítico del sistema, ya que influye directamente en:

- Cobertura
- Calidad de enlace
- Nivel de RSSI
- Relación señal/ruido (SNR)

### Observación técnica

El desempeño real depende también de:

- Altura de instalación
- Obstrucciones físicas
- Entorno urbano o abierto
- Longitud y calidad del line feeder

---

# 🌐 Interfaces de red del gateway

El gateway dispone de múltiples interfaces de red.

## 1️⃣ Interfaz de producción (uplink LoRaWAN)

- **Tipo:** Ethernet
- **Uso:** Encaminamiento de paquetes LoRaWAN hacia TTN
- **Configuración:** IP estático asignado por TI institucional

Parámetros configurados:

- IP: `143.106.8.214`
- Gateway: `143.106.8.129`
- Máscara: `255.255.255.128`
- DNS primario: `143.106.8.30`
- DNS secundario: `143.106.2.5`

La configuración se realiza en:

Network → Ethernet → IPv4 Settings
Enable WAN and Disable LAN


---

## 2️⃣ Interfaz de administración

- Acceso vía Wi-Fi local
- SSID: `RAK7289V2_BBDF`
- Dirección de administración:  
  `http://192.168.230.1`

Esta interfaz:

- Permite acceso al WisGateOS
- No participa en el tráfico LoRaWAN

---

# 🔐 Acceso al WisGateOS

El sistema operativo embarcado es **WisGateOS**, que permite:

- Configuración de red
- Selección de modo LoRaWAN
- Configuración del packet forwarder
- Diagnósticos de conectividad
- Visualización de métricas internas

Información disponible en el panel:

- Estado LoRa
- Versión de firmware
- Temperatura interna
- Estadísticas de tráfico
- Estado de conectividad

---

# 🔄 Modos de operación LoRaWAN

El gateway soporta:

- Semtech UDP Packet Forwarder (modo actual)
- LoRa Basics™ Station (no utilizado actualmente)

## Limitación operativa actual

En la versión actual de firmware:

- Cambiar modo UDP ↔ Basics Station
- Cambiar servidor LoRaWAN (TTN ↔ servidor local)

requiere:

> Factory Reset completo del gateway

No es posible modificar estos parámetros de forma incremental.

---

# ☁️ Registro en The Things Network (TTN)

## Parámetros de registro

- Gateway ID: `gateway-unicamp`
- Gateway EUI: `AC1F09FFFE0FBBDF`
- Cluster: `nam1`
- Protocolo: Semtech UDP
- Puerto UDP: **1700**
- Frequency Plan: AU915

La puerta UDP 1700 debe estar habilitada en la red institucional.

---

# 📊 Operación observada

Durante el periodo de pruebas:

- Gateway activo en TTN
- Recepción continua de uplinks
- ~264.000 uplinks registrados
- Métricas disponibles:
  - RSSI
  - SNR
  - Spreading Factor
  - Bandwidth
  - Temperatura interna

---

# ⚠️ Limitación actual de downlink

Operando en modo UDP:

- No existe canal persistente
- Downlinks no se transmiten on-air
- Dispositivos pueden retransmitir por falta de ACK

Impacto:

- Operación efectiva uplink-only
- Incremento de tráfico LoRa
- Evaluación futura de Basics Station recomendada

---

# 🔁 Procedimiento tras Factory Reset

1. Acceder al WisGateOS (`192.168.230.1`)
2. Reconfigurar Ethernet con IP estático institucional
3. Validar conectividad (`ping 8.8.8.8`)
4. Configurar modo LoRaWAN deseado
5. Apuntar al servidor correspondiente

---

# 📌 Alcance de este documento

Este documento cubre exclusivamente:

- El gateway físico RAK7289CV2
- Su conectividad de red
- Su integración con TTN

No incluye:

- Configuración de dispositivos finales
- Firmware de sensores
- Aplicaciones LoRaWAN
- Integraciones externas (tratadas en otros documentos)

---

# 📚 Referencias

- https://docs.rakwireless.com/product-categories/wisgate/rak7289v2/overview
- https://console.cloud.thethings.network


