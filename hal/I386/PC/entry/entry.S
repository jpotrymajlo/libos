



.code32


.global entry
.global _idt
.extern _start
.extern _pCurrStack
.extern _lb_rstore_regs

.global idt
.global idt_end
.global all_ints

.balign 4 ,0


.set MULTIBOOT_PAGE_ALIGN , 1<<0
.set MULTIBOOT_MEMORY_INFO, 1<<1
.set MULTIBOOT_HEADER_MAGIC, 0x1BADB002
.set MULTIBOOT_HEADER_FLAGS, MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
.set MULTIBOOT_CHECKSUM, -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)



mboot:
	 .long MULTIBOOT_HEADER_MAGIC
	 .long MULTIBOOT_HEADER_FLAGS
	 .long MULTIBOOT_CHECKSUM
      /* aout kludge. These must be PHYSICAL addresses */
/*	 .long mboot
	 .long code
	 .long bss
	 .long end
	 .long entry
*/

/*
 Multiboot header for GRUB bootloader. This must be in the first 8K
 of the kernel file. We use the aout kludge so it works with ELF,
 DJGPP COFF, Win32 PE, or other formats.
*/

/* these are in the linker script file */
.extern code, bss, end

entry:
    /* stop using bootloader GDT, and load new GDT */
	lgdt gdt_ptr

	movw $LINEAR_DATA_SEL,%ax
	movl %eax,%ds
	movl %eax,%es
	movl %eax,%ss
	movl %eax,%fs
	movl %eax,%gs
	jmpl $0x18,$sbat
sbat:
	movl $stack,%esp



	lidt idt_ptr

	call start
	jmp lb_restore_regs
	jmp .


.section .data

/*ds_magic:
	 .long DS_MAGIC
*/
gdt:
     /* NULL descriptor */
	.short 0		/* limit 15:0 */
	.short 0		/* base 15:0 */
	.byte 0		/* base 23:16      */
	.byte 0		/* type              */
	.byte 0		/* limit 19:16, flags  */
	.byte 0		/* base 31:24            */

    /*unused descriptor*/
	.short 0
	.short 0
	.byte 0
	.byte 0
	.byte 0
	.byte 0

.set LINEAR_DATA_SEL, . - gdt
	.short 0x0FFFF
	.short 0
	.byte 0
	.byte 0x92		/* present, ring 0, data, expand-up, writable*/
	.byte 0x0CF		/* page-granular (4 gig limit), 32-bit         */
	.byte 0

.set LINEAR_CODE_SEL, . - gdt
	.short 0x0FFFF
	.short 0
	.byte 0
	.byte 0x9A		/* present,ring 0,code,non-conforming,readable   */
	.byte 0x0CF		/* page-granular (4 gig limit), 32-bit             */
	.byte 0



gdt_end:

gdt_ptr:
	.short gdt_end - gdt - 1
    .long gdt


/* 50 ring 0 interrupt gates                                            */
_idt:
idt:
.rept 50
	.short 0				/* offset 15:0*/
	.short LINEAR_CODE_SEL		/* selector */
	.byte 0				/* (always 0 for interrupt gates)*/
	.byte 0x8E				/* present,ring 0,'386 interrupt gate*/
	.short 0				/* offset 31:16                        */
.endr
idt_end:

idt_ptr:
	.short idt_end - idt - 1		/* IDT limit                     */
    .long idt				/* linear adr of IDT                 */



.section .bss
	 .skip 4096
stack:
