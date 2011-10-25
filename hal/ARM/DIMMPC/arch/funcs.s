@ libgcc routines for ARM cpu.
@ Division routines, written by Richard Earnshaw, (rearnsha@armltd.co.uk)

/* Copyright 1995, 1996, 1998, 1999, 2000 Free Software Foundation, Inc.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

In addition to the permissions in the GNU General Public License, the
Free Software Foundation gives you unlimited permission to link the
compiled version of this file into combinations with other programs,
and to distribute those combinations without any restriction coming
from the use of this file.  (The General Public License restrictions
do apply in other respects; for example, they cover modification of
the file, and distribution when not linked into a combine
executable.)

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */
/* ------------------------------------------------------------------------ */

/* We need to know what prefix to add to function names.  */

#ifndef __USER_LABEL_PREFIX__
#error  __USER_LABEL_PREFIX__ not defined
#endif

/* ANSI concatenation macros.  */

#define CONCAT1(a, b) CONCAT2(a, b)
#define CONCAT2(a, b) a ## b

/* Use the right prefix for global labels.  */

#define SYM(x) CONCAT1 (__USER_LABEL_PREFIX__, x)

#define __PLT__ (PLT)
#define TYPE(x) .type SYM(x),function
#define SIZE(x) .size SYM(x), . - SYM(x)

/* Function end macros.  Variants for 26 bit APCS and interworking.  */

.macro THUMB_LDIV0
Ldiv0:
	push	{ lr }
	bl	SYM (__div0)
	mov	r0, #0			@ About as wrong as it could be.
	pop	{ pc }
.endm
/*.macro ARM_LDIV0
Ldiv0:
	str	lr, [sp, #-4]!
	bl	__div0 
	mov	r0, #0			@ About as wrong as it could be.
	ldmia	sp!, {pc}
.endm	
*/
.macro FUNC_END name no
Ldiv0\no:
	str	lr, [sp, #-4]!
	bl	__div0 
	mov	r0, #0			@ About as wrong as it could be.
	ldmia	sp!, {pc}
        .size __\name, . - __\name 
.endm

/* Function start macros.  Variants for ARM and Thumb.  */

#define THUMB_FUNC
#define THUMB_CODE
#define RET		mov	pc, lr
#define RETc(x)	mov##x	pc, lr
.macro FUNC_START name
	.text
	.globl __\name
	.type __\name,function 
	.align 0
__\name:
.endm
		
/* Register aliases.  */

work		.req	r4	@ XXXX is this safe ?
dividend	.req	r0
divisor		.req	r1
overdone	.req	r2
result		.req	r2
curbit		.req	r3
/*ip		.req	r12
sp		.req	r13
lr		.req	r14
pc		.req	r15
*/

/* ------------------------------------------------------------------------ */
/*		Bodies of the divsion and modulo routines.		    */
/* ------------------------------------------------------------------------ */	
.macro ARM_DIV_MOD_BODY modulo no
Loop1\no:
	@ Unless the divisor is very big, shift it up in multiples of
	@ four bits, since this is the amount of unwinding in the main
	@ division loop.  Continue shifting until the divisor is 
	@ larger than the dividend.
	cmp	divisor, #0x10000000
	cmplo	divisor, dividend
	movlo	divisor, divisor, lsl #4
	movlo	curbit,  curbit,  lsl #4
	blo	Loop1\no

Lbignum\no:
	@ For very big divisors, we must shift it a bit at a time, or
	@ we will be in danger of overflowing.
	cmp	divisor, #0x80000000
	cmplo	divisor, dividend
	movlo	divisor, divisor, lsl #1
	movlo	curbit,  curbit,  lsl #1
	blo	Lbignum\no

Loop3\no:
	@ Test for possible subtractions.  On the final pass, this may 
	@ subtract too much from the dividend ...
	
  .if \modulo
	@ ... so keep track of which subtractions are done in OVERDONE.
	@ We can fix them up afterwards.
	mov	overdone, #0
	cmp	dividend, divisor
	subhs	dividend, dividend, divisor
	cmp	dividend, divisor,  lsr #1
	subhs	dividend, dividend, divisor, lsr #1
	orrhs	overdone, overdone, curbit,  ror #1
	cmp	dividend, divisor,  lsr #2
	subhs	dividend, dividend, divisor, lsr #2
	orrhs	overdone, overdone, curbit,  ror #2
	cmp	dividend, divisor,  lsr #3
	subhs	dividend, dividend, divisor, lsr #3
	orrhs	overdone, overdone, curbit,  ror #3
	mov	ip,       curbit
  .else
	@ ... so keep track of which subtractions are done in RESULT.
	@ The result will be ok, since the "bit" will have been 
	@ shifted out at the bottom.
	cmp	dividend, divisor
	subhs	dividend, dividend, divisor
	orrhs	result,   result,   curbit
	cmp	dividend, divisor,  lsr #1
	subhs	dividend, dividend, divisor, lsr #1
	orrhs	result,   result,   curbit,  lsr #1
	cmp	dividend, divisor,  lsr #2
	subhs	dividend, dividend, divisor, lsr #2
	orrhs	result,   result,   curbit,  lsr #2
	cmp	dividend, divisor,  lsr #3
	subhs	dividend, dividend, divisor, lsr #3
	orrhs	result,   result,   curbit,  lsr #3
  .endif

	cmp	dividend, #0			@ Early termination?
	movnes	curbit,   curbit,  lsr #4	@ No, any more bits to do?
	movne	divisor,  divisor, lsr #4
	bne	Loop3\no

  .if \modulo
Lfixup_dividend:	
	@ Any subtractions that we should not have done will be recorded in
	@ the top three bits of OVERDONE.  Exactly which were not needed
	@ are governed by the position of the bit, stored in IP.
	ands	overdone, overdone, #0xe0000000
	@ If we terminated early, because dividend became zero, then the 
	@ bit in ip will not be in the bottom nibble, and we should not
	@ perform the additions below.  We must test for this though
	@ (rather relying upon the TSTs to prevent the additions) since
	@ the bit in ip could be in the top two bits which might then match
	@ with one of the smaller RORs.
	tstne	ip, #0x7
	beq	Lgot_result\no
	tst	overdone, ip, ror #3
	addne	dividend, dividend, divisor, lsr #3
	tst	overdone, ip, ror #2
	addne	dividend, dividend, divisor, lsr #2
	tst	overdone, ip, ror #1
	addne	dividend, dividend, divisor, lsr #1
  .endif

Lgot_result\no:
.endm
/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */
/*		Start of the Real Functions				    */
/* ------------------------------------------------------------------------ */

	FUNC_START div0
	mov	pc, lr
        FUNC_END div0 2 


	FUNC_START divsi3	

	
	eor	ip, dividend, divisor		@ Save the sign of the result.
	mov	curbit, #1
	mov	result, #0
	cmp	divisor, #0
	rsbmi	divisor, divisor, #0		@ Loops below use unsigned.
	beq	Ldiv00
	cmp	dividend, #0
	rsbmi	dividend, dividend, #0
	cmp	dividend, divisor
	blo	Lgot_result0

	ARM_DIV_MOD_BODY 0 0
	
	mov	r0, result
	cmp	ip, #0
	rsbmi	r0, r0, #0

	mov	pc, lr

        FUNC_END divsi3 0

/* ------------------------------------------------------------------------ */

	FUNC_START modsi3

	
	cmp	divisor, #0
	rsbmi	divisor, divisor, #0		@ Loops below use unsigned.
	beq	Ldiv01
	@ Need to save the sign of the dividend, unfortunately, we need
	@ ip later on; this is faster than pushing lr and using that.
	str	dividend, [sp, #-4]!
	cmp	dividend, #0			@ Test dividend against zero
	rsbmi	dividend, dividend, #0		@ If negative make positive
	cmp	dividend, divisor		@ else if zero return zero
	blo     Lgot_result1	
	mov	curbit, #1

	ARM_DIV_MOD_BODY 1 1

	ldr	ip, [sp], #4
	cmp	ip, #0
	rsbmi	dividend, dividend, #0

	mov	pc, lr

	
	FUNC_END modsi3 1



