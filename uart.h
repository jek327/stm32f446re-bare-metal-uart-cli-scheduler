#ifndef UART_H
#define UART_H

#include <stdint.h>

void UART2_Init(void);
void UART2_WriteChar(char c);
void UART2_WriteString(const char *message);
char UART2_ReadChar(void);
int  UART2_CharAvailable(void);

#endif
