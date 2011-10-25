#ifndef __ARCH_I386_H
#define	__ARCH_I386_H

/* the layout of this structure must match the order of registers
pushed and popped by the exception handlers in KSTART.ASM */

#include "types.h"

extern unsigned long OS_CPU_SR_Save(void);
extern void          OS_CPU_SR_Restore(unsigned long);
extern void          OSCtxSw(void);

typedef struct lb_registers
{
  UINT32 cpsr;
  UINT32 r0;
  UINT32 r1;
  UINT32 r2;
  UINT32 r3;
  UINT32 r4;
  UINT32 r5;
  UINT32 r6;
  UINT32 r7;
  UINT32 r8;
  UINT32 r9;
  UINT32 r10;
  UINT32 r11;
  UINT32 r12;
  UINT32 lr;
  UINT32 pc;

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
 * Instaluje funkcje obs�ugi przerwania
 *\param num numer przerwania
 *\param hnd funkcja obs�ugi przerwania
 */
void lb_set_intr(UINT32 num, void* hnd);
/**
 * Pobiera aktualny czas. Dla procesora x86 jest to warto�ci� odtrzymana po wykonaniu rdtsc
 * \return liczba cykli zegara, ktora uplynela od wlaczenia procesora
 */
UINT64 lb_get_clock(void);

/**
 * Prze�aczanie zada�
 * \hideinitializer
 * \note Makro powoduje wywy�anie systemowe, kt�rego procedur� obs�ugi wybiera
 * kolejne zadanie do wykonania z listy zada� gotowych
 */
#define SWITCH_TASK  OSCtxSw(); 

/**
  * Wy��czanie przerwa�
  * \hideinitializer
  * \param x zmienna, do kt�rej zapisany zostanie stan rejestru znacznik�w. Dla
  * x86 EFLAGS.
  * \note Makro wylacza przerwania jednoczesnie zapisujac do zmiennej ich stan przed
  * wywo�aniem
  */
#define INT_DISABLE(x) x=OS_CPU_SR_Save(); ; 
                        
                        
/** W��czanie przerwa�
  * \hideinitializer
  * \param x zmienna, do z kt�rej pobrany zostanie stan rejestru znacznik�w. Dla
  * x86 - EFLAGS.
  */
#define INT_ENABLE(x) OS_CPU_SR_Restore(x); 


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










#endif


