#ifndef DRIVER_UART_H_
#define DRIVER_UART_H_

#include "debug.h"

void Usart3_Init(void);
void Usart2_Init(void);
void Uart4_Init(void);
void Uart7_Init(void);
void uart4_send_string(u8 *string, u8 len);
void uart7_send_string(u8 *string, u8 len);
void usart1_send_string(u8 *string, u8 len);

#endif
