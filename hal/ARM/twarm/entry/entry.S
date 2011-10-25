
.equ NO_INT,   0xC0          
.equ USR_MODE, 0x10
.equ FIQ_MODE, 0x11
.equ IRQ_MODE, 0x12
.equ SVC_MODE, 0x13
.equ ABT_MODE, 0x17
.equ UND_MODE, 0x1B
.equ SYS_MODE, 0x1F

.equ USR_STACK_SIZE, 0x200
.equ FIQ_STACK_SIZE, 0x200
.equ IRQ_STACK_SIZE, 0x200
.equ SVC_STACK_SIZE, 0x200
.equ ABT_STACK_SIZE, 0x200
.equ UND_STACK_SIZE, 0x200
.equ SYS_STACK_SIZE, 0x200


.globl _start
_start:	
  b       reset
	ldr	pc, ptr_undefined_instruction
	ldr	pc, ptr_software_interrupt
	ldr	pc, ptr_prefetch_abort
	ldr	pc, ptr_data_abort
	ldr	pc, ptr_not_used
	ldr pc, [pc,#-0xF20]
	ldr	pc, ptr_fiq_isr

ptr_undefined_instruction:.word undefined_instruction
ptr_software_interrupt:	  .word software_interrupt
ptr_prefetch_abort:	      .word prefetch_abort
ptr_data_abort:		        .word data_abort
ptr_not_used:		          .word not_used
/*ptr_irq_isr: 		          .word OS_CPU_IRQ_ISR*/
ptr_fiq_isr:			        .word fiq_interrupt

.balignl 16,0xdeadbeef




reset:
       /*
        * set the cpu to SVC32 mode
	      */
        mrs     r0,cpsr
	      bic     r0,r0,#0x1f
	      orr     r0,r0,#0x13
	      msr     cpsr,r0

	     /* Disable "Fast Bus Mode" - set to async */
	      mrc p15,0,r0,c1,c0,0  
	      orr r0,r0,#0xC0000000   
	      mcr p15,0,r0,c1,c0,0    

        /* Begin by clearing out the .bss section */

        ldr r1, __bss_start
        ldr r2, __bss_end
        ldr r3, =0
clear_bss:
        cmp r1,r2
        strne r3,[r1],#+4
        bne clear_bss 

	     /*
	      * After uboot starting memories have been just remapped
	      */
	      ldr	r0, =_start
	      ldr	r1, =0x0
	      mov	r2, #16
copy_loop:
	      subs	r2, r2, #1
	      ldr	r3, [r0], #4
	      str	r3, [r1], #4
	      bne	copy_loop

setup_stacks:

        /* FIQ mode stack */
        msr   CPSR_c, #(FIQ_MODE|NO_INT)
        ldr   sp, =(fiq_stack - 4)

        /* IRQ mode stack */
        msr   CPSR_c, #(IRQ_MODE|NO_INT)
        ldr   sp, =(irq_stack - 4)


        /* Undefined mode stack */
        msr   CPSR_c, #(UND_MODE|NO_INT)
        ldr   sp, =(und_stack - 4)

        /* Abort mode stack */
        msr   CPSR_c, #(ABT_MODE|NO_INT)
        ldr   sp, =(abt_stack - 4)
        
        /* Supervisor mode stack */
        msr   CPSR_c, #(SVC_MODE|NO_INT)
        ldr   sp, =(svc_stack - 4)



        ldr     r0, =start    @ start()
        mov     lr, pc
        bx      r0
  
  
        LDR     R4, =pCurrStack         
        LDR     SP, [R4]  
                                        
        LDR     R4,  [SP], #4       
        MSR     SPSR_cxsf,R4
        LDMFD   SP!, {R0-R12,LR,PC}^    

undefined_instruction:
        STMFD   SP!, {LR}               @     Push return address
        STMFD   SP!, {LR}
        STMFD   SP!, {R0-R12}           @     Push registers
        MRS     R4,  CPSR               @     Push current CPSR
        STR     R4,  [SP, #-4]!
        mov     R0,  SP

        ldr     r1, =do_undefined_instruction    @ start()
        mov     lr, pc
        bx      r1
           
        b exception_finish
software_interrupt:
        STMFD   SP!, {LR}               @     Push return address
        STMFD   SP!, {LR}
        STMFD   SP!, {R0-R12}           @     Push registers
        MRS     R4,  CPSR               @     Push current CPSR
        STR     R4,  [SP, #-4]!
        mov     R0,  SP

        ldr     r1, =do_software_interrupt    @ start()
        mov     lr, pc
        bx      r1
           
        b exception_finish
prefetch_abort:
        STMFD   SP!, {LR}               @     Push return address
        STMFD   SP!, {LR}
        STMFD   SP!, {R0-R12}           @     Push registers
        MRS     R4,  CPSR               @     Push current CPSR
        STR     R4,  [SP, #-4]!
        mov     R0,  SP

        ldr     r1, =do_prefetch_abort    @ start()
        mov     lr, pc
        bx      r1
           
        b exception_finish
fiq_interrupt:
        STMFD   SP!, {LR}               @     Push return address
        STMFD   SP!, {LR}
        STMFD   SP!, {R0-R12}           @     Push registers
        MRS     R4,  CPSR               @     Push current CPSR
        STR     R4,  [SP, #-4]!
        mov     R0,  SP

        ldr     r1, =do_prefetch_abort    @ start()
        mov     lr, pc
        bx      r1
           
        b exception_finish        
data_abort:
        STMFD   SP!, {LR}               @     Push return address
        STMFD   SP!, {LR}
        STMFD   SP!, {R0-R12}           @     Push registers
        MRS     R4,  CPSR               @     Push current CPSR
        STR     R4,  [SP, #-4]!
        mov     R0,  SP

        ldr     r1, =do_data_abort    @ start()
        mov     lr, pc
        bx      r1
           
        b exception_finish
not_used:
        STMFD   SP!, {LR}               @     Push return address
        STMFD   SP!, {LR}
        STMFD   SP!, {R0-R12}           @     Push registers
        MRS     R4,  CPSR               @     Push current CPSR
        STR     R4,  [SP, #-4]!
        mov     R0,  SP

        ldr     r1, =do_not_used    @ start()
        mov     lr, pc
        bx      r1
           
        b exception_finish
        
exception_finish:
        b exception_finish        



__bss_start:   .word _bss_start
__bss_end:     .word _bss_end


.section ".bss"
.align  4

        .word 0
        .space IRQ_STACK_SIZE
irq_stack:
        .word 0        
        .space FIQ_STACK_SIZE
fiq_stack:
        .word 0
        .space UND_STACK_SIZE
und_stack:
        .word 0
        .space ABT_STACK_SIZE
abt_stack:
        .word 0
        .space SVC_STACK_SIZE
svc_stack:
        .word 0
        .space SYS_STACK_SIZE
sys_stack:
        .word 0


