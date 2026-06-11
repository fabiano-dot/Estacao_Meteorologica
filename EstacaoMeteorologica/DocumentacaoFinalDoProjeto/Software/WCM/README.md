# Esp32-C3

## Comando para verificar acesso ao Esp32
$ .espressif/python_env/idf6.0_py3.13_env/bin/esptool --chip esp32c3 --port /dev/ttyACM0 chip-id


## Comando para apagar Flash
$ .espressif/python_env/idf6.0_py3.13_env/bin/esptool --chip esp32c3 --port /dev/ttyACM0 erase-flash


## Comando para gravar Flash
$ .espressif/python_env/idf6.0_py3.13_env/bin/esptool.py --chip esp32c3 --port /dev/ttyACM0 --baud 460800 write_flash 0x0 lmic_abp_00.ino.merged.bin


## Versões:
- Build 0004: Testado na semana 12-16/01/2026
- Build 0005: Incorpora OTAA ainda não testado