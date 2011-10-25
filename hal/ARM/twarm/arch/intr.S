@********************************************************************************************************
@                                               uC/OS-II
@                                         The Real-Time Kernel
@
@                               (c) Copyright 1992-2005, Micrium, Weston, FL
@                                          All Rights Reserved
@
@                                           Generic ARM Port
@
@ File      : OS_CPU_A.ASM
@ Version   : V1.60
@ By        : Jean J. Labrosse
@ GNU Port  : Jean-Denis Hatier
@
@ For       : ARM7 or ARM9
@ Mode      : ARM or Thumb
@ Toolchain : GNU GCC
@********************************************************************************************************

            .extern  OSRunning                   @ External references
            .extern  OSPrioCur
            .extern  OSPrioHighRdy
            .extern  OSTCBCur
            .extern  OSTCBHighRdy
            .extern  OS_CPU_IRQ_ISR_Handler
            .extern  OS_CPU_FIQ_ISR_Handler
            .extern  sched_reschedule
            .extern  irq_c_handler


            .global  OS_CPU_SR_Save              @ Functions declared in this file
            .global  OS_CPU_SR_Restore
            .global  OSStartHighRdy
            .global  OSCtxSw
            .global  OSIntCtxSw
            .global  OS_CPU_IRQ_ISR
            .global  OS_CPU_FIQ_ISR
            .global  irq_timer_isr


            .equ NO_INT,     0xC0                @ Mask used to disable interrupts (Both FIR and IRQ)
            .equ SVC32_MODE, 0x13
            .equ FIQ32_MODE, 0x11
            .equ IRQ32_MODE, 0x12


@*********************************************************************************************************
@                                   CRITICAL SECTION METHOD 3 FUNCTIONS
@
@ Description: Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
@              would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
@              disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
@              disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
@              into the CPU's status register.
@
@ Prototypes :     OS_CPU_SR  OS_CPU_SR_Save(void);
@                  void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
@
@
@ Note(s)    : 1) These functions are used in general like this:
@
@                 void Task (void *p_arg)
@                 {
@                 #if OS_CRITICAL_METHOD == 3          /* Allocate storage for CPU status register */
@                     OS_CPU_SR  cpu_sr;
@                 #endif
@
@                          :
@                          :
@                     OS_ENTER_CRITICAL();             /* cpu_sr = OS_CPU_SaveSR();                */
@                          :
@                          :
@                     OS_EXIT_CRITICAL();              /* OS_CPU_RestoreSR(cpu_sr);                */
@                          :
@                          :
@                 }
@
@              2) OS_CPU_SaveSR() is implemented as recommended by Atmel's application note:
@
@                    "Disabling Interrupts at Processor Level"
@*********************************************************************************************************

        .code 32

OS_CPU_SR_Save:
        MRS     R0,CPSR                     @ Set IRQ and FIQ bits in CPSR to disable all interrupts
        ORR     R1,R0,#NO_INT
        MSR     CPSR_c,R1
        MRS     R1,CPSR                     @ Confirm that CPSR contains the proper interrupt disable flags
        AND     R1,R1,#NO_INT
        CMP     R1,#NO_INT
        BNE     OS_CPU_SR_Save              @ Not properly disabled (try again)
        BX      LR                          @ Disabled, return the original CPSR contents in R0


OS_CPU_SR_Restore:
        MSR     CPSR_c,R0
        BX      LR

@*********************************************************************************************************
@                                          START MULTITASKING
@                                       void OSStartHighRdy(void)
@
@ Note(s) : 1) OSStartHighRdy() MUST:
@              a) Call OSTaskSwHook() then,
@              b) Set OSRunning to TRUE,
@              c) Switch to the highest priority task.
@*********************************************************************************************************



 

@*********************************************************************************************************
@                         PERFORM A CONTEXT SWITCH (From task level) - OSCtxSw()
@
@ Note(s) : 1) OSCtxSw() is called in SYS mode with BOTH FIQ and IRQ interrupts DISABLED
@
@           2) The pseudo-code for OSCtxSw() is:
@              a) Save the current task's context onto the current task's stack
@              b) OSTCBCur->OSTCBStkPtr = SP;
@              c) OSTaskSwHook();
@              d) OSPrioCur             = OSPrioHighRdy;
@              e) OSTCBCur              = OSTCBHighRdy;
@              f) SP                    = OSTCBHighRdy->OSTCBStkPtr;
@              g) Restore the new task's context from the new task's stack
@              h) Return to new task's code
@
@           3) Upon entry:
@              OSTCBCur      points to the OS_TCB of the task to suspend
@              OSTCBHighRdy  points to the OS_TCB of the task to resume
@*********************************************************************************************************

        .code 32

OSCtxSw:
                                        @ SAVE CURRENT TASK'S CONTEXT
        STMFD   SP!, {LR}               @     Push return address
        STMFD   SP!, {LR}
        STMFD   SP!, {R0-R12}           @     Push registers
        MRS     R4,  CPSR               @     Push current CPSR
        STR     R4,  [SP, #-4]!
        
        LDR     R4, =pCurrStack
        STR     SP, [R4]
        
        LDR     R0, =sched_reschedule    @ OS_CPU_IRQ_ISR_Handler();
        MOV     LR, PC
        BX      R0

        LDR     R4, =pCurrStack       @    Get highest priority task TCB address
        LDR     SP, [R4]             @ SP = OSTCBHighRdy->OSTCBStkPtr;

        LDR     R4,  [SP], #4           @    pop new task's CPSR
        MSR     SPSR_cxsf,R4
        LDMFD   SP!, {R0-R12,LR,PC}^    @    pop new task's context

 @*********************************************************************************************************
@                   PERFORM A CONTEXT SWITCH (From interrupt level) - OSIntCtxSw()
@
@ Note(s) : 1) OSIntCtxSw() is called in SYS mode with BOTH FIQ and IRQ interrupts DISABLED
@
@           2) The pseudo-code for OSCtxSw() is:
@              a) OSTaskSwHook();
@              b) OSPrioCur             = OSPrioHighRdy;
@              c) OSTCBCur              = OSTCBHighRdy;
@              d) SP                    = OSTCBHighRdy->OSTCBStkPtr;
@              e) Restore the new task's context from the new task's stack
@              f) Return to new task's code
@
@           3) Upon entry:
@              OSTCBCur      points to the OS_TCB of the task to suspend
@              OSTCBHighRdy  points to the OS_TCB of the task to resume
@*********************************************************************************************************


@*********************************************************************************************************
@                                      IRQ Interrupt Service Routine
@*********************************************************************************************************

        .code 32
irq_timer_isr:    

        STMFD   SP!, {R1-R3}                   @ PUSH WORKING REGISTERS ONTO IRQ STACK
        MOV     R1, SP                         @ Save   IRQ stack pointer
        ADD     SP, SP,#12                     @ Adjust IRQ stack pointer
        SUB     R2, LR,#4                      @ Adjust PC for return address to task
        MRS     R3, SPSR                       @ Copy SPSR (i.e. interrupted task's CPSR) to R3
        MSR     CPSR_c, #(NO_INT | SVC32_MODE) @ Change to SVC mode

        STMFD   SP!, {R2}                      @    Push task's Return PC
        STMFD   SP!, {LR}                      @    Push task's LR
        STMFD   SP!, {R4-R12}                  @    Push task's R12-R4

        LDMFD   R1!, {R4-R6}                   @    Move task's R1-R3 from IRQ stack to SVC stack
        STMFD   SP!, {R4-R6}
        STMFD   SP!, {R0}                      @    Push task's R0    onto task's stack
        STMFD   SP!, {R3}                      @    Push task's CPSR (i.e. IRQ's SPSR)

                
        LDR     R4, =pCurrStack
        STR     SP, [R4]

       MSR     CPSR_c, #(NO_INT | IRQ32_MODE) @ Change to IRQ mode (to use the IRQ stack to handle interrupt)
        
        BL      irq_c_handler     
        
        MSR     CPSR_c, #(NO_INT | SVC32_MODE) @ Change to SVC mode
        
        LDR     R4, =pCurrStack       @    Get highest priority task TCB address
        LDR     SP, [R4]             @ SP = OSTCBHighRdy->OSTCBStkPtr;


                                               @ RESTORE NEW TASK'S CONTEXT
        LDMFD   SP!, {R4}                      @    Pop new task's CPSR
        MSR     SPSR_cxsf, R4
        LDMFD   SP!, {R0-R12,LR,PC}^           @    Pop new task's context

        
        