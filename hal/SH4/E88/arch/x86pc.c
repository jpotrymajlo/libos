#include <stdio.h>
#include <arch.h>
#include <libos.h>
#include <tulip/tulip.h>
#include <pci/pci.h>


extern LB_INT_GATE idt[];

void lb_set_intr(UINT32 int_num, void* hnd)
{

	LB_INT_GATE* pGate = &idt[int_num];

	pGate->offset_l   = (UINT16)((UINT32)hnd & 0x0000ffff);
	pGate->offset_h  = (UINT16)((UINT32)hnd >> 16);
	pGate->selector = 0x18;
	pGate->type     = 0x8e00;
}

UDWORD* stack_init(void (*task)(void),void *ptos)
{
    UDWORD* stack = (UDWORD*)ptos;
//    *--stack = arg; 
//    *--stack = 0x00000000; 
    *--stack = 0x00000202;

    *--stack = 0x00000018;			/* CS */
    *--stack = (UDWORD) task;	/* Entry point */

    *--stack = 0x00000000;       /*xxx*/
    *--stack = 0x00000000;       /*xxx*/
  

    *--stack = 0x00000010;       /*GS*/
    *--stack = 0x00000010;       /*FS*/
    *--stack = 0x00000010;       /*ES*/
    *--stack = 0x00000010;       /*DS*/


    *--stack = 0x00000000;       /*EAX*/
    *--stack = 0x00000000;       /*ECX*/
    *--stack = 0x00000000;       /*EDX*/
    *--stack = 0x00000000;       /*EBX*/
    *--stack = 0x00000000;
    *--stack = 0x00000000;       /*EBP*/
    *--stack = 0x00000000;       /*ESI*/
    *--stack = 0x00000000;       /*EDI*/

    *--stack = 0x00000000;       /*xxx*/
    
     return stack;
}


void lb_def_handler(LB_REGISTERS* lb_regs)
{
     printk("nie obsluzone przerwanie\n");
     printk("eax - %x",lb_regs->eax);
     while (1);
}



/*inicjalizacja zegara systemowego
przerwanie co 8ms*/
void init_8253(void)
{
    outportb(0x43,0x36);
	outportb(0x40,0x49);
	outportb(0x40,0x25);
}
void init_8259(void)
{
	static const unsigned irq0_int = 0x20, irq8_int = 0x28;
/**/

/* Initialization Control Word #1 (ICW1) */
	outportb(0x20, 0x11);
	outportb(0xA0, 0x11);
/* ICW2:
route IRQs 0-7 to INTs 20h-27h */
	outportb(0x21, irq0_int);
/* route IRQs 8-15 to INTs 28h-2Fh */
	outportb(0xA1, irq8_int);
/* ICW3 */
	outportb(0x21, 0x04);
	outportb(0xA1, 0x02);
/* ICW4 */
	outportb(0x21, 0x01);
	outportb(0xA1, 0x01);
/* enable IRQ0 (timer) and IRQ1 (keyboard) */
	outportb(0x21, ~0x13);
	outportb(0xA1, ~0x00);
}
/*inicjalizacja ukladu UART*/
void init_8250(void)
{
/*    set_uart_reg(REG_LCR,0x07);
    set_uart_reg(REG_LSB,0x0C);
    set_uart_reg(REG_MSB,0x00);
    set_uart_reg(REG_IMR,INTR_ON_BYTE);*/

}

extern void create_video(void);

void lb_init_hw(void)
{

    int i;

    for (i=0;i<50;i++){
        lb_set_intr((UINT32)i,lb_def_isr);
    }
    lb_set_intr(0x08,lb_def_isr_ex);
    lb_set_intr(0x0a,lb_def_isr_ex);
    lb_set_intr(0x0b,lb_def_isr_ex);
    lb_set_intr(0x0c,lb_def_isr_ex);
    lb_set_intr(0x0d,lb_def_isr_ex);
    lb_set_intr(0x0e,lb_def_isr_ex);

    lb_set_intr(0x20,lb_timer_isr);
    lb_set_intr(0x21,lb_kbd_isr);
    lb_set_intr(0x30,sched_reschedule_isr);



    init_8253();
    init_8250();
	init_8259();

}

void outportb(unsigned int port, unsigned int val)
{
	__asm__ __volatile__("outb %b0,%w1" : : "a"(val), "d"(port));
}
void outportw(unsigned short port,unsigned short value)
{
    __asm__ __volatile__ ("outw %w0,%w1" : : "a" (value), "Nd" (port));
}
void outportl(unsigned short port,unsigned int value)
{
    __asm__ __volatile__ ("outl %0,%w1" : : "a" (value), "Nd" (port));
}



unsigned int inportb(unsigned short port)
{
	unsigned char ret_val;
	__asm__ __volatile__("inb %1,%0" : "=a"(ret_val): "d"(port));
	return ret_val;
}

unsigned short inportw(unsigned short port)
{
    unsigned short _v;
    __asm__ __volatile__ ("inw %w1,%0" : "=a" (_v): "Nd" (port) );
    return _v;
};
unsigned int inportl(unsigned short port)
{
    unsigned int _v;
    __asm__ __volatile__ ("inl %w1,%0" : "=a" (_v): "Nd" (port) );
    return _v;
};



static UINT64 lb_prev_time;





UINT64 lb_get_clock(void)
{
    UINT64 clock64;
    __asm__ __volatile__ ("rdtsc" : "=A"(clock64));
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

void lb_delay(UINT32 wait)
{
  unsigned int i, j;
  wait /= 100;
  for (j=0; j<wait ; j++)
    for (i=0; i<100; i++)
      outportb(0x80, 0);	/* write is # 1us */

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


