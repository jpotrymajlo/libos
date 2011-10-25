.section .text


.extern do_irq_timer
.extern sched_reschedule
.extern lb_kbd_intr
/*.extern _lb_dec21140_intr*/
.extern lb_def_handler
.extern pCurrStack

.global lb_timer_isr
.global sched_reschedule_isr
.global lb_kbd_isr
.global lb_dec21140_isr
.global lb_def_isr
.global lb_def_isr_ex
.global lb_restore_regs

lb_timer_isr:
	push $0
	push $0
	push %gs
	push %fs
	push %es
	push %ds
	pusha

	mov %esp,%eax
	push %eax
    mov %esp,%eax
    mov %eax,pCurrStack

    call do_irq_timer

    mov pCurrStack,%eax
    mov %eax,%esp
	pop %eax
	popa				/* pop GP registers                     */
	pop %ds				/* pop segment registers                */
	pop %es
	pop %fs
	pop %gs
	add $8,%esp
	iret

sched_reschedule_isr:
	push $0
	push $0
	push %gs
	push %fs
	push %es
	push %ds
	pusha

	mov %esp,%eax
	push %eax
    mov %esp,%eax
    mov %eax,pCurrStack

    call sched_reschedule
	jmp lb_restore_regs

lb_kbd_isr:
	push $0
	push $0
	push %gs
	push %fs
	push %es
	push %ds
	pusha

	mov %esp,%eax
	push %eax
    mov %esp,%eax
    mov %eax,pCurrStack
    call lb_kbd_intr
	jmp lb_restore_regs

/*_lb_dec21140_isr:
	push $0
	push $0
	push %gs
	push %fs
	push %es
	push %ds
	pusha

	mov %esp,%eax
	push %eax
    mov %esp,%eax
    mov %eax,pCurrStack
    call _lb_dec21140_intr
	jmp lb_restore_regs*/

/* domyslna obsluga przerwanie bez odkladania na stos kodu bledu */
lb_def_isr:  
	push $0
	push $0
	push %gs
	push %fs
	push %es
	push %ds
	pusha

	mov %esp,%eax
	push %eax
    mov %esp,%eax
    mov %eax,pCurrStack

    call lb_def_handler

	jmp lb_restore_regs


/* domyslna obsluga przerwanie bez odkladania na stos kodu bledu */
lb_def_isr_ex:  
	push $0
	push %gs
	push %fs
	push %es
	push %ds
	pusha

	mov %esp,%eax
	push %eax
    mov %esp,%eax
    mov %eax,pCurrStack

    call lb_def_handler
	jmp lb_restore_regs


lb_restore_regs:
    mov pCurrStack,%eax
    mov %eax,%esp
	pop %eax
	popa				/* pop GP registers                     */
	pop %ds				/* pop segment registers                */
	pop %es
	pop %fs
	pop %gs
	add $8,%esp
	iret
    
