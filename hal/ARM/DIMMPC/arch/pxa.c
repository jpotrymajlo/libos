#include <stdio.h>
#include <arch.h>
#include <libos.h>
#include <uart.h>
#include <mmu.h>

#define GET_VAL_32(ADDR)  *((uint32*)(ADDR))
#define GET_VAL_16(ADDR)  *((uint16*)(ADDR))
#define SET_VAL_32(ADDR, VAL) *((uint32*)(ADDR)) = (VAL);
#define SET_VAL_16(ADDR, VAL) *((uint16*)(ADDR)) = (VAL);


void ARCH_Init(void)
{
   char dbg_array[128];
   uint32  val;
   uint32  val2;
   uint32  val3;
   uint16  val16;
   
//   SET_VAL(0xa0000000, 0x1234);
   MMU_Disable();
   MMU_InitPageTable(); 
   MMU_Enable();

//   SET_VAL(0x00000004, 0x5678);
     
   UART_Init();
//   val = GET_VAL(0xa0000004);
   val16 = GET_VAL_16(0x50000000);
   sprintf(dbg_array, "%X\n" , val16);
   UART_Puts(dbg_array);
         
   while (1)
     ;
//   start();
}





void outportb(unsigned int port, unsigned int val)
{
}

void outportw(unsigned short port,unsigned short value)
{
}
void outportl(unsigned short port,unsigned int value)
{
}



unsigned int inportb(unsigned short port)
{
}

unsigned short inportw(unsigned short port)
{
};
unsigned int inportl(unsigned short port)
{
};

UDWORD* stack_init(void (*task)(void),void *ptos)
{
  return NULL;
}

UINT64 div64(UINT64 num,UINT64 den)
{
  return 0;
}

void lb_init_hw(void)
{

}

void lb_cpu_init_counter(void)
{

}
