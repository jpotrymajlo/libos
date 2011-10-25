#include <libos.h>
#include <uart/uart.h>


UINT32 std_write(UINT8* buff, INT32 size)
{
  return uart_write(buff, size);
}