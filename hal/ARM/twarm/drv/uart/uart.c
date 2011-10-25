
#include <libos.h>
#include <at91rm9200_full.h>
#include <bits.h>
#include <tty/tty.h>



#define us  ((AT91PS_USART) AT91C_BASE_DBGU)
void putc (const char c)
{
  if (c == '\n')
    putc ('\r');
  while ((us->US_CSR & AT91C_US_TXRDY) == 0);
  us->US_THR = c;
}


void puts(const char* s)
{
  while (*s)
  {
    if (*s == '\n')
      putc('\r');
    putc(*s);
    s++;
   }
}

void uart(void)
{

}

void uart_read(void)
{

}

UINT32 uart_write(UINT8* buff, INT32 size)
{
  while (size--) {
	while ((us->US_CSR & AT91C_US_TXRDY) == 0);
	us->US_THR = *buff;
	if (*buff == '\n'){
      while ((us->US_CSR & AT91C_US_TXRDY) == 0);
	  us->US_THR = '\r';	  
	}
	buff++;
  }
  return 0;
}

void uart_irq(void)
{
  
  UBYTE key;
  msg_in_t* pMsg;


  if ((pMsg = msg_accept_msg(&ExInputIO)) != NULL){
      pMsg->bMsgData[0] = AT91C_BASE_DBGU->US_RHR;
      pMsg->dwMsgLength = 1;
      pMsg->dwMsgType = MSG_IO_IN;
      msg_send_msg_intr(&ExTty,(msg_t*)pMsg);
      AT91C_BASE_DBGU->US_CR = AT91C_US_RSTSTA; 
      sched_reschedule();
  }  
}
