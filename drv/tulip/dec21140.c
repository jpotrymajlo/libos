#include <libos.h>
#include <arch.h>
#include <stdio.h>
#include <stdlib.h>
#include <pci/pci.h>
#include <pci/linuxpci.h>
#include "tulip.h"

static UINT8     device;
static UINT8     bus;
static UINT32    tulip_port;
static UINT32*   tulip_base;
static UINT8     tulip_int;
static UINT32    valdword;
static UINT8     mac_address[6];
static struct MD  ringBuffer[NTXBUFS+NRXBUFS];
static struct MD* rx_base_MD;
static struct MD* tx_base_MD;
static UINT8     rxBuffers1[NRXBUFS][RBUF_SIZE];
static UINT8     txBuffers1[NTXBUFS][TBUF_SIZE];
static struct MD*       tx_MD;
static struct MD*       rx_MD;


/*
 * This routine reads a word (16 bits) from the serial EEPROM.
 */
/*  EEPROM_Ctrl bits. */
#define EE_SHIFT_CLK		0x02	/* EEPROM shift clock. */
#define EE_CS               0x01	/* EEPROM chip select. */
#define EE_DATA_WRITE       0x04	/* EEPROM chip data in. */
#define EE_WRITE_0		    0x01
#define EE_WRITE_1		0x05
#define EE_DATA_READ		0x08	/* EEPROM chip data out. */
#define EE_ENB			(0x4800 | EE_CS)


/* The EEPROM commands include the alway-set leading bit. */
#define EE_WRITE_CMD	(5 << 6)
#define EE_READ_CMD     (6 << 6)
#define EE_ERASE_CMD	(7 << 6)

#define SET_TULIP_REG(ADDR,VALUE) *(tulip_base+(ADDR))=VALUE
#define GET_TULIP_REG(ADDR)       *(tulip_base+(ADDR))

static void st_le32(volatile UINT32 *addr,UINT32 value)
{
  *(addr)=value ;
}

static UINT32 ld_le32(volatile UINT32 *addr)
{
  return(*addr);
}


extern void lb_dec21140_isr(void);

/*#define rtems_bsp_delay_in_bus_cycles(cycle) lb_delay( cycle )*/
#define rtems_bsp_delay_in_bus_cycles(cycle) 

static int eeget16(volatile UINT32 *ioaddr, int location)
{
   int i;
   UINT16 retval = 0;
   int read_cmd = location | EE_READ_CMD;

   st_le32(ioaddr, EE_ENB & ~EE_CS);
   st_le32(ioaddr, EE_ENB);

   /* Shift the read command bits out. */
   for (i = 10; i >= 0; i--) {
      short dataval = (read_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
      st_le32(ioaddr, EE_ENB | dataval);
      rtems_bsp_delay_in_bus_cycles(200);
      st_le32(ioaddr, EE_ENB | dataval | EE_SHIFT_CLK);
      rtems_bsp_delay_in_bus_cycles(200);
      st_le32(ioaddr, EE_ENB | dataval); /* Finish EEPROM a clock tick. */
      rtems_bsp_delay_in_bus_cycles(200);
   }
   st_le32(ioaddr, EE_ENB);

   for (i = 16; i > 0; i--) {
      st_le32(ioaddr, EE_ENB | EE_SHIFT_CLK);
      rtems_bsp_delay_in_bus_cycles(200);
      retval = (retval << 1) | ((ld_le32(ioaddr) & EE_DATA_READ) ? 1 : 0);
      st_le32(ioaddr, EE_ENB);
      rtems_bsp_delay_in_bus_cycles(200);
   }

   /* Terminate the EEPROM access. */
   st_le32(ioaddr, EE_ENB & ~EE_CS);
   return ( ((retval<<8)&0xff00) | ((retval>>8)&0xff) );
}

void lb_dec21140_intr(void)
{
   UINT32               status;

   /*
    * Read status
    */
   status = GET_TULIP_REG(memCSR5);
   SET_TULIP_REG(memCSR5,status);

   /*
    * Frame received?
    */
   if( status & 0x000000c0 )
   {
       printk("Rx Eth frame\n");
   }
}

int dec21140_init(void)
{

   UINT32     i;
   struct MD* rmd;
   UINT8*     setup_frm;
   
   if(!pci_find_device(PCI_VENDOR_ID_DEC,PCI_DEVICE_ID_DEC_21140,&bus,&device))
       return ERROR;

   printk("DEC 21140 PCI network card found:\n");
   printk("    dec21140 bus  %b\n",bus);
   printk("    dec21140 dev  %b\n",device);

   pcibios_read_config_dword(bus,device,16,&tulip_port);
   tulip_port &= (UINT32)~IO_MASK;
   printk("    dec21140 port %d\n",tulip_port);

   pcibios_read_config_dword(bus,device,20,&valdword);
   tulip_base = (UINT32*)(valdword & (~MEM_MASK));
   printk("    dec21140 base %d\n",tulip_base);

   pcibios_read_config_byte(bus,device,60,&tulip_int);
   printk("    dec21140 int  %b\n",tulip_int);

   /*
   ** Prep the board
   */

   pcibios_write_config_word(bus,device,PCI_COMMAND,
                         (UINT16)( PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER));




   /*
   ** Setup ethernet address
   */
      printk("    dec21140 MAC  ");

      union {char c[64]; UINT16 s[32];} rombuf;

      for (i=0; i<32; i++){
         rombuf.s[i] = eeget16(tulip_base + memCSR9,i);
      }
      for (i=0;i<3;i++){
         mac_address[2*i]   = rombuf.c[20+2*i+1];
         mac_address[2*i+1] = rombuf.c[20+2*i];
         printk("%b %b ",mac_address[2*i],mac_address[2*i+1]);
      }
          
      printk("\n");


    SET_TULIP_REG(memCSR6,CSR6_INIT);
    SET_TULIP_REG(memCSR0,RESET_CHIP);
    lb_delay(200);

    SET_TULIP_REG(memCSR7,NO_IT);
    SET_TULIP_REG(memCSR0,CSR0_MODE);

   /*
    * Init RX ring
    */

   rmd = ringBuffer;
   rx_base_MD = rmd;

   SET_TULIP_REG(memCSR3,(UINT32)rx_base_MD);

   for (i=0;i<NRXBUFS-1;i++,rmd++)
   {
      rmd->buf1   = (UINT32)(rxBuffers1[i][0]);
      rmd->buf2   = (UINT32)(rmd+1);
      rmd->status = 0x80000000;
      rmd->counts = 0xfdc00000 | (RBUF_SIZE);
      rmd->next   = rmd+1;
      rmd->m = &rxBuffers1[i][0];
   }
   rmd->buf1   = (UINT32)(rxBuffers1[i][0]);
   rmd->buf2   = 0;
   rmd->status = 0x80000000;
   rmd->counts = 0xfec00000 | (RBUF_SIZE);
   rmd->next   = rx_base_MD;
   rmd->m      = &rxBuffers1[i][0];

   printk("    dec21140 bufor odbiorczy zainicjowany\n");

      /*
    * Init TX ring
    */
   tx_base_MD = ++rmd;
   SET_TULIP_REG(memCSR4,(UINT32)tx_base_MD);

   for (i=0,rmd++;i<NTXBUFS-1; i++){
      rmd->buf2   = (UINT32)(rmd+1);
      rmd->buf1   = (UINT32)(txBuffers1[i][0]);
      rmd->counts = 0x01000000;
      rmd->status = 0x0;
      rmd->next   = rmd+1;
	  rmd->m      = 0;
   }
   rmd->buf2   = (UINT32)(tx_base_MD);
   rmd->buf1   = (volatile UINT32)(txBuffers1[i][0]);
   rmd->counts = 0x01000000;
   rmd->status = 0x0;
   rmd->next   = tx_base_MD;
   rmd->m      = 0;

   printk("    dec21140 bufor nadawczy zainicjowany\n");

   lb_set_intr(0x2b,lb_dec21140_isr);

   printk("    dec21140 zainstalowana obsluga przerwania\n");

   /*
    * Build setup frame
    */
   setup_frm = (UINT8*)(rmd->buf1);

   /* Fill the buffer with our physical address. */
   for (i = 1; i < 16; i++) {
      *setup_frm++ = mac_address[0];
      *setup_frm++ = mac_address[1];
      *setup_frm++ = mac_address[0];
      *setup_frm++ = mac_address[1];
      *setup_frm++ = mac_address[2];
      *setup_frm++ = mac_address[3];
      *setup_frm++ = mac_address[2];
      *setup_frm++ = mac_address[3];
      *setup_frm++ = mac_address[4];
      *setup_frm++ = mac_address[5];
      *setup_frm++ = mac_address[4];
      *setup_frm++ = mac_address[5];
   }

    /* Add the broadcast address when doing perfect filtering */
    memset((void*) setup_frm, 0xff, 12);
    tx_base_MD->counts = 0x09000000 | 192 ;
    tx_base_MD->status = 0x80000000;
    SET_TULIP_REG(memCSR6,CSR6_INIT | CSR6_TX);
    SET_TULIP_REG(memCSR1,1);

    while (tx_base_MD->status != 0x7fffffff){
        tx_base_MD->counts = 0x01000000;
    }


    tx_MD = tx_base_MD+1;
    printk("    dec21140 setup frame\n");

   /*
    * Enable RX and TX
    */

    SET_TULIP_REG(memCSR5, IT_SETUP);
    SET_TULIP_REG(memCSR7, IT_SETUP);
    SET_TULIP_REG(memCSR6, CSR6_INIT | CSR6_TXRX);


   
    return 1;
}


