

#ifndef TULIP_H_
#define TULIP_H_

/* note: the 21143 isn't really a DEC, it's an Intel chip */
#define PCI_INVALID_VENDORDEVICEID	    0xffffffff
#define PCI_VENDOR_ID_DEC               0x1011
#define PCI_DEVICE_ID_DEC_21140         0x0009
#define PCI_DEVICE_ID_DEC_21143         0x0019

#define DRIVER_PREFIX   "dc"

#define IO_MASK   0x3
#define MEM_MASK  0xF

#define NRXBUFS 16	/* number of receive buffers */
#define NTXBUFS 16	/* number of transmit buffers */


/*
 * Number of DEC boards supported by this driver
 */
#define NDECDRIVER	1

/*
 * Receive buffer size -- Allow for a full ethernet packet including CRC
 */
#define RBUF_SIZE	1536
#define TBUF_SIZE   1536
#define MCLBYTES (2*RBUF_SIZE)

#define	ET_MINLEN       60	/* minimum message length */

/* command and status registers, 32-bit access, only if IO-ACCESS */
#define ioCSR0  0x00	/* bus mode register */
#define ioCSR1  0x08	/* transmit poll demand */
#define ioCSR2  0x10	/* receive poll demand */
#define ioCSR3  0x18	/* receive list base address */
#define ioCSR4  0x20	/* transmit list base address */
#define ioCSR5  0x28	/* status register */
#define ioCSR6  0x30	/* operation mode register */
#define ioCSR7  0x38	/* interrupt mask register */
#define ioCSR8  0x40	/* missed frame counter */
#define ioCSR9  0x48	/* Ethernet ROM register */
#define ioCSR10 0x50	/* reserved */
#define ioCSR11 0x58	/* full-duplex register */
#define ioCSR12 0x60	/* SIA status register */
#define ioCSR13 0x68
#define ioCSR14 0x70
#define ioCSR15 0x78	/* SIA general register */

/* command and status registers, 32-bit access, only if MEMORY-ACCESS */
#define memCSR0  0x00	/* bus mode register */
#define memCSR1  0x02	/* transmit poll demand */
#define memCSR2  0x04	/* receive poll demand */
#define memCSR3  0x06	/* receive list base address */
#define memCSR4  0x08	/* transmit list base address */
#define memCSR5  0x0A	/* status register */
#define memCSR6  0x0C	/* operation mode register */
#define memCSR7  0x0E	/* interrupt mask register */
#define memCSR8  0x10	/* missed frame counter */
#define memCSR9  0x12	/* Ethernet ROM register */
#define memCSR10 0x14	/* reserved */
#define memCSR11 0x16	/* full-duplex register */
#define memCSR12 0x18	/* SIA status register */
#define memCSR13 0x1A
#define memCSR14 0x1C
#define memCSR15 0x1E	/* SIA general register */

#define DEC_REGISTER_SIZE    0x100   /* to reserve virtual memory */

#define RESET_CHIP   0x00000001
#define CSR0_MODE    0x0020e002   /* 01b08000 */
#define ROM_ADDRESS  0x00004800
#define CSR6_INIT    0x022cc000   /* 022c0000 020c0000 */
#define CSR6_TX      0x00002000
#define CSR6_TXRX    0x00002002
#define IT_SETUP     0x000100c0   /* 000100e0 */
#define CLEAR_IT     0xFFFFFFFF
#define NO_IT        0x00000000

/* message descriptor entry */
struct MD {
    /* used by hardware */
    volatile UINT32   status;
    volatile UINT32   counts;
    volatile UINT32   buf1, buf2;
    /* used by software */
    volatile void *m; /* zmienic na strukture mbuf */
    volatile struct MD *next;
} __attribute__ ((packed));

extern int dec21140_init(void);

#endif
