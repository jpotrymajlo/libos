#ifndef __ARCH_I386_H
#define	__ARCH_I386_H

/* the layout of this structure must match the order of registers
pushed and popped by the exception handlers in KSTART.ASM */

#include "types.h"

typedef struct lb_registers
{
/* pushed by pusha */
	UINT32 edi;
    UINT32 esi;
    UINT32 ebp;
    UINT32 esp;
    UINT32 ebx;
    UINT32 edx;
    UINT32 ecx;
    UINT32 eax;
/* pushed separately */
	UINT32 ds;
    UINT32 es;
    UINT32 fs;
    UINT32 gs;
	UINT32 int_num;
    UINT32 err_code;
/* pushed by exception. Exception may also push err_code.
user_esp and user_ss are pushed only if a privilege change occurs. */
	UINT32 eip;
    UINT32 cs;
    UINT32 eflags;
    UINT32 user_esp;
    UINT32 user_ss;
}LB_REGISTERS;


/* the code for setvect() and getvect() in
KSTART.ASM depends on the layout of this structure */


typedef struct lb_int_gate
{
	WORD	offset_l;
	WORD	selector;
	WORD	type;
	WORD	offset_h;
}LB_INT_GATE;



/**
 * \defgroup arch Funkcje niskopoziomowe
 * @{
 */

/**
 * Instaluje funkcje obs³ugi przerwania
 *\param num numer przerwania
 *\param hnd funkcja obs³ugi przerwania
 */
void lb_set_intr(UINT32 num, void* hnd);
/**
 * Pobiera aktualny czas. Dla procesora x86 jest to wartoœci¹ odtrzymana po wykonaniu rdtsc
 * \return liczba cykli zegara, ktora uplynela od wlaczenia procesora
 */
UINT64 lb_get_clock(void);

/**
 * Prze³aczanie zadañ
 * \hideinitializer
 * \note Makro powoduje wywy³anie systemowe, którego procedurê obs³ugi wybiera
 * kolejne zadanie do wykonania z listy zadañ gotowych
 */
#define SWITCH_TASK  ; 

/**
  * Wy³¹czanie przerwañ
  * \hideinitializer
  * \param x zmienna, do której zapisany zostanie stan rejestru znaczników. Dla
  * x86 EFLAGS.
  * \note Makro wylacza przerwania jednoczesnie zapisujac do zmiennej ich stan przed
  * wywo³aniem
  */
#define INT_DISABLE(x)  ; 
                        
                        
/** W³¹czanie przerwañ
  * \hideinitializer
  * \param x zmienna, do z której pobrany zostanie stan rejestru znaczników. Dla
  * x86 - EFLAGS.
  */
#define INT_ENABLE(x) ; 


/** @}*/

void outportb(unsigned  port, unsigned val);
void outportw(unsigned short port,unsigned short value);
void outportl(unsigned short port,unsigned int value);


unsigned int inportb(unsigned short port);
unsigned short inportw(unsigned short port);
unsigned int inportl(unsigned short port);
void lb_delay(UINT32 wait);
void     init_8250(void);
void     init_8253(void);
void     init_8259(void);

void lb_cpu_init_counter(void);
UINT16 lb_cpu_stop_counter(void);
void lb_cpu_start_counter(void);
UINT64 lb_get_clock(void);
UINT64 lb_get_diff(void);
UINT64 div64(UINT64 num,UINT64 den);
void lb_init_hw(void);
void lb_timer_isr(void);
void sched_reschedule_isr(void);
void lb_kbd_isr(void);
void lb_def_isr(void);
void lb_def_isr_ex(void);







#define TIMER_INT  0x20
#define SWITCH_INT 0x30



#endif


