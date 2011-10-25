#ifndef UART_H_
#define UART_H_

UINT32 uart_write(UINT8* buff, INT32 size);
void   uart_irq(void);

#endif /*UART_H_*/
