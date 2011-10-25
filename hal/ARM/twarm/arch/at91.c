#include <libos.h>
#include <version.h>
#include "at91rm9200_full.h"
#include "bits.h"
#include <uart/uart.h>
extern void irq_timer_isr(void);


void arch_AciInit(void)
{
    *AT91C_AIC_IDCR                   = 0xFFFFFFFF;                     /* Disable ALL interrupts.         */
}

struct pt_regs
{
  long uregs[18];
};





/*void do_irq (struct pt_regs *pt_regs)*/
void irq_c_handler(void)
{
  unsigned long  reg_val;


    reg_val = *AT91C_ST_SR;
    if (reg_val & AT91C_ST_PITS) {                  /* If the interrupt is from the tick source,       */
      irq_timer();
    }
    if(AT91C_BASE_DBGU->US_CSR & AT91C_US_RXRDY)
    {
      uart_irq();
    }
    *AT91C_AIC_EOICR = BIT1;                        /* End of interrupt handler.                       */
}





/*#define BSP_AT91RM9200_CLK  12768*/
#define BSP_AT91RM9200_CLK  32768 

void arch_InterruptsInit(void)
{
//  AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, AT91C_ID_SYS, AT91C_AIC_PRIOR_LOWEST, AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, irq_timer_isr);
     //* Disable the interrupt on the interrupt controller
  

  //* Disable the interrupt on the interrupt controller
  AT91C_BASE_AIC->AIC_IDCR = 0x01 << AT91C_ID_SYS;
  //* Save the interrupt handler routine pointer and the interrupt priority
  AT91C_BASE_AIC->AIC_SVR[AT91C_ID_SYS] = (unsigned int) irq_timer_isr ;
  //* Store the Source Mode Register
  AT91C_BASE_AIC->AIC_SMR[AT91C_ID_SYS] = AT91C_AIC_PRIOR_LOWEST | AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE;
  //* Clear the interrupt on the interrupt controller
  AT91C_BASE_AIC->AIC_ICCR = 0x01 << AT91C_ID_SYS;

  AT91C_BASE_AIC->AIC_IECR    = BIT1;                          /* Enable timer interrupt at AIC level.        */

                                                      /* Initialize the timer to generate 100 Hz.    */
  AT91C_BASE_ST->ST_PIMR     = BSP_AT91RM9200_CLK / 100;
  AT91C_BASE_ST->ST_IER      = AT91C_ST_PITS;                 /* Enable timer interrupt at ST level.         */
  AT91C_BASE_DBGU->US_IER = AT91C_US_RXRDY;           /* Enable RX ready interrupt */    


}




void __div0(void)
{

}
#define  ARM_MODE_ARM     0x00000000
#define  ARM_MODE_THUMB   0x00000020

                                                  /* __CPU_MODE__ is an IAR built-in constant indicating whether ... */
                                                  /* ... code of this file was compiled using ARM or Thumb mode      */
#define  ARM_SVC_MODE    (0x00000013L + ARM_MODE_ARM)
UDWORD* stack_init(void (*task)(void),void *ptos)
{
    UDWORD* stk;


    stk      = ptos;                        /* Load stack pointer                                      */
    *(--stk) = (UDWORD)task;                /* Entry Point                                             */
    *(--stk) = (UDWORD)0x14141414L;         /* R14 (LR)                                                */
    *(--stk) = (UDWORD)0x12121212L;         /* R12                                                     */
    *(--stk) = (UDWORD)0x11111111L;         /* R11                                                     */
    *(--stk) = (UDWORD)0x10101010L;         /* R10                                                     */
    *(--stk) = (UDWORD)0x09090909L;         /* R9                                                      */
    *(--stk) = (UDWORD)0x08080808L;         /* R8                                                      */
    *(--stk) = (UDWORD)0x07070707L;         /* R7                                                      */
    *(--stk) = (UDWORD)0x06060606L;         /* R6                                                      */
    *(--stk) = (UDWORD)0x05050505L;         /* R5                                                      */
    *(--stk) = (UDWORD)0x04040404L;         /* R4                                                      */
    *(--stk) = (UDWORD)0x03030303L;         /* R3                                                      */
    *(--stk) = (UDWORD)0x02020202L;         /* R2                                                      */
    *(--stk) = (UDWORD)0x01010101L;         /* R1                                                      */
    *(--stk) = (UDWORD)0x000000000;         /*p_arg;                R0 : argument                                           */
    *(--stk) = (UDWORD)ARM_SVC_MODE;        /* CPSR  (Enable both IRQ and FIQ interrupts)              */

    return (stk);
}

void lb_init_hw(void)
{
	

  arch_AciInit();
  //arch_UsartInit();
  arch_InterruptsInit(); 
	 
}



static UINT64 lb_prev_time;

UINT64 lb_get_clock(void)
{
    UINT64 clock64;
    return clock64;
}

UINT64 lb_get_diff(void)
{
    UINT64 act;
    UINT64 diff;
    act = lb_get_clock();
    diff = act-lb_prev_time;
    lb_prev_time = act;
    return diff;

}

void lb_cpu_init_counter(void)
{
    lb_get_diff();
}

void arch_os_init(void)
{
		
}

UINT64 div64(UINT64 num,UINT64 den)
{
    UINT64 div=0;
    UINT64 mod=0;
    UINT64 count=64;
    UINT64 carry;

    if (num==0)
        return 0;
    if (den==0)
        return 0;
    while (!(num & 0x8000000000000000ULL)){
         count--;
         num<<=1;
    }
    while (count){
        carry = (num & 0x8000000000000000ULL)>>63;
        num=num<<1;
        mod=(mod<<1)+carry;

        if (mod>=den){
            mod=mod-den;
            div=(div<<1)+1;
        }else{
            div=div<<1;
        }
        count--;
    }
    return div;
}
