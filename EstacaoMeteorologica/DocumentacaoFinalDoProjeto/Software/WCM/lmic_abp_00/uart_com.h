#ifndef UART_COM_H
#define UART_COM_H

typedef enum{
  UART_ST_WAITING_DATA = 0,
  UART_ST_DATA_READY,
  UART_ST_DATA_ERROR,
  UART_ST_TIMEOUT
}UartState;

extern HardwareSerial SerialUART;

void      uart_setup();
void      uart_start_read_data();
UartState uart_state();

#endif // UART_COM_H