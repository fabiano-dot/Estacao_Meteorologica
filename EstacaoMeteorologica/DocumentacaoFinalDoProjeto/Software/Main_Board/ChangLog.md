# Estação Meteorológica LoRa/LoRaWAN
## Next:
- Melhorar estrutura do loop_printf quando o USB está off
- Manter histórico na flash do fcnt
- Verificar se e possivel melhorar a estabilidade da USB no modo OFF-ON
- rever funcionamento GPS, principalmente durante sleep
- Compartilhar código com esp32-c3

- wcm melhorar protocolo de envio/recebimento dos dados para o WCM
    - Aguardar OK de acordado
    - Criar Tipos de Mensagem de retorno
- Remover driver ina
- Doxigen pendentes:
    - aq_data_gps
    - fcnt
    - store
    
1-avaliar payload de 32 bits
3-melhoria comunicação esp32
mudar fcnt de 16 para 32bits? menu e wcm
Corrigir o build:0037 para funcionar no debug


## Build:0042 - 07/04/2026
- Add modos channel AUTO 0-7, 8-15 e 0-15

## Build:0041 - 19/02/2026
- Corrigido erro da  correção do bh1750

## Build:0040 - 18/02/2026
- Atualização do README.md para o WCM
- desativado START_SEND_WITH_FIX_DATA
- Correção, em caso de falha na leitura i2c do bh1750, repetia o último valor, não reportava o erro;

## Build:0039 - 15/02/2026
- Correção do número da versão

## Build:0038 - 15/02/2026
- Ativado doxigen

## Build:0037 - 15/02/2026
- Add config para transmitir fcnt do ABP com 4 ou 8 Bytes

## Build:0036 - 12/02/2026
- Desativado NoFlash, pois é necessário para funcionar o storage
- write flash com 8 bytes travava, usando 256 bytes

## Build:0035 - 12/02/2026
- Add file fcnt
- Removido: LorawanAbpPar.fcnt

## Build:0034 - 12/02/2026 - Rev02
- Version = 0.00.02
- removido:
    ActiveSensors.battery
    AqData.battery
    files aq_data_bat


## Build:0033 - 11/02/2026
- Doxigen:
    - hw_sleep

## Build:0032 - 08/02/2026
- Correção, faltava desligar o pll_sys no sleep

## Build:0030 - 08/02/2026
- refactory da rotina de geração do CMD do arquivo wcm.c
- Add opção de adicionar checksum ao payload
- Add janela para pressionar botão
- Doxigen:
    - wcm
    - storage

## Build:0029 - 07/02/2026
- Move conteudo das rotinas que estavam dentro do dir src/gps para aq_data_gps

## Build:0028 - 07/02/2026
- Doxigen:
    - main
    - aq_data
    - aq_data_ad
    - aq_data_bmep280
    - aq_data_lux
    - buttons_and_leds
    - code_config
    - est_config
    - loop_printf
    - menu-conf
    - wrap_watchdog
    
## Build:0027 - 07/02/2026
- gps rever printf para loop_print
- Criação do ChangeLog.md, estes dados foram para lá
- README.md com a descrição do funcionamento do programa (iniciado)
- Inicio da documentação com Doxygen

## Build:0026 - 06/02/2026
- Pequenos ajustes no MPU Temp
- Correção da primeira leitura do BME280
- BMEP280 suporta leitura tanto no BME280 como no BMP280
- Correçao nas defines de DEBUG do code_config.h

## Build:0025 - 06/02/2026
- Corrigido msg de erro do bme280
- Ajustado layout do printf:
  - de dados que serão enviados ao WCM
  - aq_data_lux
  - aq_data_ad
- Redução do consumo do BH1750 de 115uA para 3,7 uA

## Build:0024 - 01/02/2026
- USB TurnOn parametrizado
- Correção do calculo da pressão de bme280

## Build:0023 - 31/01/2026
- Correções do watchdog

## Build:0022 - 30/01/2026
- Opção de sempre ativar o menu após reset (util para detectar resets/travamentos)
- Add prints de parâmetros de compilação
- criação da rotina get_t_now() para que timer_hw->timerawl se transforme em uma variável de 64 bits
- Menu_conf: revisão de testos, limitado o modo LoRa em ABP e OTAA
- Botão Config, inverte a função do always menu
- Add Watchdog

## Build:0021 - 28/01/2026
- Adicionado no menu/sistema um parâmetro de calibração vsys_k_10000
- removido antigos src do bmp280
- rotinas i2c do luximetro revisto o blocking por timeout

## Build:0020 - 28/01/2026
- Luximetro:
    - foi reduzida a sensibilidade para 31/69 para poder suportar incidencias diretas do sol
    - foi adicionado no menu/sistema um parâmetro de calibração lux_k_10000
    - removido o antigo driver bh1750
- Correções no WCM.c para o VSys

## Build:0019 - 27/01/2026
- correção: Não salvava a versão da configuração
- Unificada as rotinas de ad: cpu temp e vsys
- corrigido a falta de? adc_gpio_init(ADC_VSYS_GPIO);

## Build:0018 - 24/01/2026
- Add aq_data_bmep280.h
Obs.: - roda só o bmep280 para testes e está com prints de debug
      - Está com o debug ativo e só em flash

## Build:0017 - 22/01/2026
- Add VSys

## Build:0016 - 17/01/2026
- Add ao Payload lux
- Add no estado original da flash, no printf e no menu_conf
- Refactore sensor tbo por cpu_temp
- Change default channel to 8

## Build:0015 - 15/01/2026
- Add no log das chamadas ao WCM o char que retornou
- Add driver do luximetro bh1750
- Add Timeout de transmissão

## Build:0014 - 14/01/2026
- Rotina de on-off do PLL USB agora leva em conta não só a USB mas também se o AD está On ou Off
- Implementada rotinas para comunicação LoRa e LoRaWANa OTAA
- Corrigido ordem do address no ABP

## Build:0013 - 14/01/2026
- Removido arquivos flash .h e .c

## Build:0012 - 14/01/2026
- Migrado da estação original as rotinas storage.h .c
- Add Funcionalidade no menu para Read Write configuração e clear all

## Build:0011unic - 13/01/2026
- Códigos default da instalação da Unicamp

## Build:0011 - 13/01/2026
- Correções no sensor CPU temp

## Build:0010 - 13/01/2026
- Criado: int aqdata_init_power_on( AqData *value)
- Transformado em parâmetro a interrupção da UART do GPS
- No menu:
  - Add CPU temp
  - Mudança de "Sleep time" para "Sampling time"

## Build:0009 - 11/01/2026
- Pequenas melhorias no menu_conf.c
- Adicionado controle de log(ativação) da USB: OFF, ON, OFF-ON
- Adicionado controle de LEDs acessos para reduzir consumo
- Obs.: wcm_send está documentado

## Build:0008- 10/01/2026
- replace printf por loop_printf para printf s que são acessados dentro do loop, para poder desativar a USB

## Build:0007- 10/01/2026
- Refactory variáveis de configuração
- Add Mode Lora
- Revisto menu

## Build:0006- 10/01/2026
- Transmitindo LoRaWAM ABP
- Baixo consumo quando em idle 

## Build:0004- 04/01/2026
- Menu parcialmente funcional para LoRaWAN ABP variaveis do lorawan
- Rotinas com sensores de:
    - Bateria
    - Pressão, temperatura (BMP280)
    - GPS
- Sleep mode
- Comunicação com WCM para envio LoRaWAN ABP


## Build:0002- 08/12/2025
- Add simple Sleep mode
- Add simple Bat    Driver
- Add simple Bmp280 Driver
- Add simple GPS    Driver
- Add print do que deve ser enviado pelo loRa
- Add Debug Mode

## Build:0001- 05/12/2025
- inicio do menu
- estrutura do main e todos os arquivos necessários
- Estrutura básica da configuração
- Estrutura básica dos dados
- Definição do payload físico:
[Byte de controle] [dados....] [Byte de controle] [dados....] [Byte de controle] [dados....]

BC[0] ==> Byte de controle 1 define o que está presente e o tamanho de cada item

Bit 0 ==> Battery Level, Add 1 Byte no Payload
    Add 1 Byte: Batery Level 0-100 %, 0xFF ==> erro no sensor

Bit 1 ==> Bme280, Add 5 Bytes no Payload,
    Add 1 Byte:  Umidade:       0 -  100 %,       Resolução  0.5 %
        0xFF ==> erro no sensor
        0x00 ==>   0.0 %
        0x01 ==>   0.5 %
        0x02 ==>   1.0 %
        0xC8 ==> 100.0 %
        Umidade(em %) = Valor * 0.5;

    Add 2 Bytes: Temperatura: -40 -  +85 Celsius, Resolução 0.01 C
        0x7FFF ==> erro no sensor
        0xD8F0
        -10000 ==> -100.00 C
        0xFFFF ==>   -0.01 C
        0x0000 ==>    0.00 C
        0x0001 ==>    0.01 C
        0x2710
         10000 ==> +100 0.00 C
        Temperatura (em Celsius) = Valor * 0.01
        T = V / 100

    Add 2 Bytes: Pressão:     300 - 1100 hPa,     Resolução 0.02 hPa
        0xFFFF ==> erro no sensor
        0x0000 ==> 1200.00 hPa
        0x0001 ==> 1199.98 hPa
        0xAFC8
         45000 ==>  300.00 hPa
        Pressão(em hPa) = (60000 - Valor) * 0.02
    
Bit 2 ==> GPS, Add 10 Bytes
    Add x 4 Bytes Latitude
        0x7FFF FFFF ==> erro no sensor
        latitude em graus = valor * 512 /(2^32)
    Add x 4 Bytes Longitude
        0x7FFF FFFF ==> erro no sensor
        latitude em graus = valor * 512 /(2^32)
    Add x 2 Bytes Altitude
        0x7FFF ==> erro no sensor
        atitude em metros = valor * 0.1


A serem definidos:
Bit 3 ==> Luxímetro (2 bytes)
Bit 4 ==> UV índex (1 Byte)
Bit 5 ==> vento(direção (1byte, 2 graus de resolução) e velocidade (1byte, resolução de 0,2 m/s para vMax  51m/2 ou 0,72km/h com Vmax 183 km/h)) 
Bit 6 ==> Pluviômetro
Bit 7 ==> 


BC[1] ==> Byte de controle 2, pode ou não existir para definir outros campos com o byte de controle 1


## Build:0000-Base 05/12/2025
Wizard Raspberry Pi Pico para Pico 
