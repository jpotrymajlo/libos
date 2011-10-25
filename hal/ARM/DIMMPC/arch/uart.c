/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * Copyright (C) 1999 2000 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <types.h>
#include <pxa-regs.h>

unsigned int quot;

static void uart_config(void)
{
  quot = 24;

  CKEN |= CKEN6_FFUART;
        
  FFIER = 0;  /* Disable for now */
  FFFCR = 0;  /* No fifos enabled */

  /* set baud rate */
  FFLCR = LCR_WLS0 | LCR_WLS1 | LCR_DLAB;
  FFDLL = quot & 0xff;
  FFDLH = quot >> 8;
  FFLCR = LCR_WLS0 | LCR_WLS1;

  FFIER = IER_UUE;  /* Enable FFUART */
}


/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 *
 */
void UART_Init(void)
{
  uart_config();
}


/*
 * Output a single byte to the serial port.
 */
void UART_Putc(const char c)
{
  /* wait for room in the tx FIFO on FFUART */
  while((FFLSR & LSR_TEMT) == 0) ;

  FFTHR = c;
  /* If \n, also do \r */
  if(c == '\n')
    UART_Putc('\r');
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int UART_Tstc(void)
{
  return FFLSR & LSR_DR;
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int UART_Getc(void)
{
  while (!(FFLSR & LSR_DR))
    ;
  return (char)FFRBR & 0xff;
}

void UART_Puts (const char *s)
{
  while (*s) {
    UART_Putc(*s++);
  }
}
