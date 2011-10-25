/* Project:     HARTIK 3.0 Network Library                      */
/* Description: Hard Real TIme Kernel for 8086 compatible       */
/* Author:      Luca Abeni					*/
/* Date:        1/6/1998                                        */

/* File: 	PCI.C						*/
/* Revision:    2.00						*/

/*
   This module containes functions an structures used to read the PCI
   configuration registers and scan all the PCI devices in the system.
   This module was witen as a support for the net driver, (for probing
   the PCI network cards), but is also usefull for other purpouse (probing
   of graphics cards, PCI controllers,...)

   Revision 2.00: Linux compatibility
*/

#include <arch.h>
#include <stdio.h>
#include "pci.h"
#include "linuxpci.h"

int (*pcibios_read_config_byte)(BYTE bus, BYTE dev, BYTE where, BYTE *val);
int (*pcibios_read_config_word)(BYTE bus, BYTE dev, BYTE where, WORD *val);
int (*pcibios_read_config_dword)(BYTE bus, BYTE dev, BYTE where, DWORD *val);
int (*pcibios_write_config_byte)(BYTE bus, BYTE dev, BYTE where, BYTE val);
int (*pcibios_write_config_word)(BYTE bus, BYTE dev, BYTE where, WORD val);
int (*pcibios_write_config_dword)(BYTE bus, BYTE dev, BYTE where, DWORD val);

static int PCItype = 0;
static int ndev = 0;
static struct pci_des pci_devs[N_MAX_DEVS];

/* LowLevel Functions							*/
int pci_conf1_read_config_dword(BYTE bus, BYTE dev, BYTE where, DWORD *val)
{
    if (where & 3) return 0;
   outportl(0x0CF8, 0x80000000 | (bus <<16) | (dev << 8) | (where & ~3));
   *val = inportl(0x0CFC);
   return 1;
}

int pci_conf2_read_config_dword(BYTE bus, BYTE dev, BYTE where, DWORD *val)
{
    if (dev & 0x80)
	return -1;
    outportw(0xCF8, (((dev &  7) << 1) | 0xf0));
    outportw(0xCFA, bus);
    *val = inportl(0xC000 | ((dev & 0x78) << 5) + where);
    outportw(0xCF8,0);
    return 1;
}

int pci_conf1_read_config_byte(BYTE bus, BYTE dev, BYTE where, BYTE *val)
{
   outportl(0x0CF8, 0x80000000 | (bus <<16) | (dev << 8) | (where & ~3));
   *val = inportb(0x0CFC + (where & 3));
   return 1;
}

int pci_conf2_read_config_byte(BYTE bus, BYTE dev, BYTE where, BYTE *val)
{
    if (dev & 0x80)
	return -1;
    outportw(0xCF8, (((dev &  7) << 1) | 0xf0));
    outportw(0xCFA, bus);
    *val = inportb(0xC000 | ((dev & 0x78) << 5) + where);
    outportw(0xCF8,0);
    return 1;
}
int pci_conf1_read_config_word(BYTE bus, BYTE dev, BYTE where, WORD *val)
{
    if (where & 1) return 0;
    outportl(0x0CF8, 0x80000000 | (bus <<16) | (dev << 8) | (where & ~3));
    *val = inportw(0x0CFC + (where & 2));
    return 1;
}

int pci_conf2_read_config_word(BYTE bus, BYTE dev, BYTE where, WORD *val)
{
    if (dev & 0x80)
	return -1;
    outportw(0xCF8, (((dev &  7) << 1) | 0xf0));
    outportw(0xCFA, bus);
    *val = inportw(0xC000 | ((dev & 0x78) << 5) + where);
    outportw(0xCF8,0);
    return 1;
}


int pci_conf1_write_config_dword(BYTE bus, BYTE dev, BYTE where, DWORD val)
{
    if (where & 3) return 0;
   outportl(0x0CF8, 0x80000000 | (bus <<16) | (dev << 8) | (where & ~3));
   outportl(0x0CFC, val);
   return 1;
}

int pci_conf2_write_config_dword(BYTE bus, BYTE dev, BYTE where, DWORD val)
{
    if (dev & 0x80)
	return -1;
    outportw(0xCF8, (((dev &  7) << 1) | 0xf0));
    outportw(0xCFA, bus);
    outportl(0xC000 | ((dev & 0x78) << 5) + where, val);
    outportw(0xCF8,0);
    return 1;
}

int pci_conf1_write_config_byte(BYTE bus, BYTE dev, BYTE where, BYTE val)
{
   outportl(0x0CF8, 0x80000000 | (bus <<16) | (dev << 8) | (where & ~3));
   outportb(0x0CFC + (where & 3), val);
   return 1;
}

int pci_conf2_write_config_byte(BYTE bus, BYTE dev, BYTE where, BYTE val)
{
    if (dev & 0x80)
	return -1;
    outportw(0xCF8, (((dev &  7) << 1) | 0xf0));
    outportw(0xCFA, bus);
    outportb(0xC000 | ((dev & 0x78) << 5) + where, val);
    outportw(0xCF8,0);
    return 1;
}
int pci_conf1_write_config_word(BYTE bus, BYTE dev, BYTE where, WORD val)
{
    if (where & 1) return 0;
    outportl(0x0CF8, 0x80000000 | (bus <<16) | (dev << 8) | (where & ~3));
    outportw(0x0CFC + (where & 2), val);
    return 1;
}

int pci_conf2_write_config_word(BYTE bus, BYTE dev, BYTE where, WORD val)
{
    if (dev & 0x80)
	return -1;
    outportw(0xCF8, (((dev &  7) << 1) | 0xf0));
    outportw(0xCFA, bus);
    outportw(0xC000 | ((dev & 0x78) << 5) + where, val);
    outportw(0xCF8,0);
    return 1;
}



int pci_check_direct()
{
    int PCItype;
    DWORD tmp;

    PCItype = 0;
    outportb(0xCFB, 0x01);
    tmp = inportl(0xCF8);
    outportl(0xCF8, 0x80000000);
    if (inportl(0xCF8) == 0x80000000) {
	outportl(0xCF8, tmp);
	pcibios_read_config_dword = pci_conf1_read_config_dword;
	pcibios_read_config_word = pci_conf1_read_config_word;
	pcibios_read_config_byte = pci_conf1_read_config_byte;

	pcibios_write_config_dword = pci_conf1_write_config_dword;
	pcibios_write_config_word = pci_conf1_write_config_word;
	pcibios_write_config_byte = pci_conf1_write_config_byte;
	PCItype = 1;
    }
    outportw(0x0CF8,tmp);

    outportb(0xCFB, 0);
    outportb(0xCF8, 0);
    outportb(0x0CFA,0);
    if ((inportb(0x0CF8) == 0) && (inportb(0x0CFA) == 0)) {
	    pcibios_read_config_dword = pci_conf2_read_config_dword;
	    pcibios_read_config_word = pci_conf2_read_config_word;
	    pcibios_read_config_byte = pci_conf2_read_config_byte;

	    pcibios_write_config_dword = pci_conf2_write_config_dword;
	    pcibios_write_config_word = pci_conf2_write_config_word;
	    pcibios_write_config_byte = pci_conf2_write_config_byte;
	    PCItype = 2;
    }
    return PCItype;
}

/*__________________________________________________________________________*/
/*__________________________________________________________________________*/
int pcibios_present(void)
{
	return (PCItype != 0);
}

int pcibios_init(void)
{

    PCItype = pci_check_direct();
    if (PCItype == 0) return -1;
    return 1;
}
    
/* scan the bus to find all the connected devices */
int pci_scan_bus(struct pci_des *p_des)
{
    int ndev;
    DWORD tmp;
    BYTE hdr;
    int j, ok;
    BYTE bus, b;
    WORD dev;
    int present;

    if (PCItype == 0) return -1;
    ndev = 0;

    for (bus = 0; bus <= 0; bus++) {
	for (dev = 0; dev <= 0xFF; dev++) {
	    present = 0;
	    if ((dev & 0x07) == 0) {
		present = pcibios_read_config_byte(bus, dev, 0x0e, &hdr);
	    }
	    if (hdr & 0x80) {
		present = 1;
	    }
	    if (present) {
		ok = pcibios_read_config_dword(bus, dev, 0, &tmp);
		if ( ok && (((tmp & 0xFFFF) != 0xFFFF) && ((tmp >> 16) != 0xFFFF))) {
		    p_des[ndev].bus = bus;
		    p_des[ndev].dev = dev;
		    for (j = 0; j <= 255; j++) {
			pcibios_read_config_byte(bus, dev, j, &b);
			p_des[ndev].mem[j] = b;
		    }
		    ndev++;
		}	
	    }
	}
    }
    return ndev;
}

struct pci_regs *pci_find_device(WORD vendor, WORD device, BYTE *bus, BYTE *dev)
{
    int i;
    struct pci_regs *regs;

    for (i = 0; i < ndev; i++) {
	regs = (struct pci_regs *)pci_devs[i].mem;
	if ((regs->VendorId == vendor) && (regs->DeviceId == device)) {
		*dev = pci_devs[i].dev;
		*bus = pci_devs[i].bus;
		return regs;
	    }

	}
    
    return NULL;
}

struct pci_regs *pci_find_class(DWORD class_code, BYTE *bus, BYTE *dev)
{
    int i;
    struct pci_regs *regs;

    for (i = 0; i < ndev; i++) {
	    regs = (struct pci_regs *)pci_devs[i].mem;
	    if ((regs->ClassCode << 8) == class_code) {
		    *dev = pci_devs[i].dev;
		    *bus = pci_devs[i].bus;
		    return regs;
	    }
    }
    return NULL;
}

int pcibios_find_device(WORD vendor, WORD device, BYTE *bus, BYTE *dev)
{

	if (pci_find_device(vendor, device, bus, dev) != NULL) {
		return PCIBIOS_SUCCESSFUL;
	} else {
		return PCIBIOS_DEVICE_NOT_FOUND;
	}
}
int pcibios_find_class(DWORD class_code, BYTE *bus, BYTE *dev)
{

	if (pci_find_class(class_code, bus, dev) != NULL) {
		return PCIBIOS_SUCCESSFUL;
	} else {
		return PCIBIOS_DEVICE_NOT_FOUND;
	}
}
/***************************************************************************/
/*		This is from Linux!!!					   */
/***************************************************************************/
const char *pci_strclass (unsigned int class)
{
	switch (class >> 8) {
	      case PCI_CLASS_NOT_DEFINED:		return "Non-VGA device";
	      case PCI_CLASS_NOT_DEFINED_VGA:		return "VGA compatible device";

	      case PCI_CLASS_STORAGE_SCSI:		return "SCSI storage controller";
	      case PCI_CLASS_STORAGE_IDE:		return "IDE interface";
	      case PCI_CLASS_STORAGE_FLOPPY:		return "Floppy disk controller";
	      case PCI_CLASS_STORAGE_IPI:		return "IPI bus controller";
	      case PCI_CLASS_STORAGE_RAID:		return "RAID bus controller";
	      case PCI_CLASS_STORAGE_OTHER:		return "Unknown mass storage controller";

	      case PCI_CLASS_NETWORK_ETHERNET:		return "Ethernet controller";
	      case PCI_CLASS_NETWORK_TOKEN_RING:	return "Token ring network controller";
	      case PCI_CLASS_NETWORK_FDDI:		return "FDDI network controller";
	      case PCI_CLASS_NETWORK_ATM:		return "ATM network controller";
	      case PCI_CLASS_NETWORK_OTHER:		return "Network controller";

	      case PCI_CLASS_DISPLAY_VGA:		return "VGA compatible controller";
	      case PCI_CLASS_DISPLAY_XGA:		return "XGA compatible controller";
	      case PCI_CLASS_DISPLAY_OTHER:		return "Display controller";

	      case PCI_CLASS_MULTIMEDIA_VIDEO:		return "Multimedia video controller";
	      case PCI_CLASS_MULTIMEDIA_AUDIO:		return "Multimedia audio controller";
	      case PCI_CLASS_MULTIMEDIA_OTHER:		return "Multimedia controller";

	      case PCI_CLASS_MEMORY_RAM:		return "RAM memory";
	      case PCI_CLASS_MEMORY_FLASH:		return "FLASH memory";
	      case PCI_CLASS_MEMORY_OTHER:		return "Memory";

	      case PCI_CLASS_BRIDGE_HOST:		return "Host bridge";
	      case PCI_CLASS_BRIDGE_ISA:		return "ISA bridge";
	      case PCI_CLASS_BRIDGE_EISA:		return "EISA bridge";
	      case PCI_CLASS_BRIDGE_MC:			return "MicroChannel bridge";
	      case PCI_CLASS_BRIDGE_PCI:		return "PCI bridge";
	      case PCI_CLASS_BRIDGE_PCMCIA:		return "PCMCIA bridge";
	      case PCI_CLASS_BRIDGE_NUBUS:		return "NuBus bridge";
	      case PCI_CLASS_BRIDGE_CARDBUS:		return "CardBus bridge";
	      case PCI_CLASS_BRIDGE_OTHER:		return "Bridge";

	      case PCI_CLASS_COMMUNICATION_SERIAL:	return "Serial controller";
	      case PCI_CLASS_COMMUNICATION_PARALLEL:	return "Parallel controller";
	      case PCI_CLASS_COMMUNICATION_OTHER:	return "Communication controller";

	      case PCI_CLASS_SYSTEM_PIC:		return "PIC";
	      case PCI_CLASS_SYSTEM_DMA:		return "DMA controller";
	      case PCI_CLASS_SYSTEM_TIMER:		return "Timer";
	      case PCI_CLASS_SYSTEM_RTC:		return "RTC";
	      case PCI_CLASS_SYSTEM_OTHER:		return "System peripheral";

	      case PCI_CLASS_INPUT_KEYBOARD:		return "Keyboard controller";
	      case PCI_CLASS_INPUT_PEN:			return "Digitizer Pen";
	      case PCI_CLASS_INPUT_MOUSE:		return "Mouse controller";
	      case PCI_CLASS_INPUT_OTHER:		return "Input device controller";

	      case PCI_CLASS_DOCKING_GENERIC:		return "Generic Docking Station";
	      case PCI_CLASS_DOCKING_OTHER:		return "Docking Station";

	      case PCI_CLASS_PROCESSOR_386:		return "386";
	      case PCI_CLASS_PROCESSOR_486:		return "486";
	      case PCI_CLASS_PROCESSOR_PENTIUM:		return "Pentium";
	      case PCI_CLASS_PROCESSOR_ALPHA:		return "Alpha";
	      case PCI_CLASS_PROCESSOR_POWERPC:		return "Power PC";
	      case PCI_CLASS_PROCESSOR_CO:		return "Co-processor";

	      case PCI_CLASS_SERIAL_FIREWIRE:		return "FireWire (IEEE 1394)";
	      case PCI_CLASS_SERIAL_ACCESS:		return "ACCESS Bus";
	      case PCI_CLASS_SERIAL_SSA:		return "SSA";
	      case PCI_CLASS_SERIAL_USB:		return "USB Controller";
	      case PCI_CLASS_SERIAL_FIBER:		return "Fiber Channel";

	      default:					return "Unknown class";
	}
}


const char *pci_strvendor(unsigned int vendor)
{
	switch (vendor) {
	      case PCI_VENDOR_ID_COMPAQ:	return "Compaq";
	      case PCI_VENDOR_ID_NCR:		return "NCR";
	      case PCI_VENDOR_ID_ATI:		return "ATI";
	      case PCI_VENDOR_ID_VLSI:		return "VLSI";
	      case PCI_VENDOR_ID_ADL:		return "Advance Logic";
	      case PCI_VENDOR_ID_NS:		return "NS";
	      case PCI_VENDOR_ID_TSENG:		return "Tseng'Lab";
	      case PCI_VENDOR_ID_WEITEK:	return "Weitek";
	      case PCI_VENDOR_ID_DEC:		return "DEC";
	      case PCI_VENDOR_ID_CIRRUS:	return "Cirrus Logic";
	      case PCI_VENDOR_ID_IBM:		return "IBM";
	      case PCI_VENDOR_ID_WD:		return "Western Digital";
	      case PCI_VENDOR_ID_AMD:		return "AMD";
	      case PCI_VENDOR_ID_TRIDENT:	return "Trident";
	      case PCI_VENDOR_ID_AI:		return "Acer Incorporated";
	      case PCI_VENDOR_ID_MATROX:	return "Matrox";
	      case PCI_VENDOR_ID_CT:		return "Chips & Technologies";
	      case PCI_VENDOR_ID_MIRO:		return "Miro";
	      case PCI_VENDOR_ID_FD:		return "Future Domain";
	      case PCI_VENDOR_ID_SI:		return "Silicon Integrated Systems";
	      case PCI_VENDOR_ID_HP:		return "Hewlett Packard";
	      case PCI_VENDOR_ID_PCTECH:	return "PCTECH";
	      case PCI_VENDOR_ID_DPT:		return "DPT";
	      case PCI_VENDOR_ID_OPTI:		return "OPTI";
	      case PCI_VENDOR_ID_SGS:		return "SGS Thomson";
	      case PCI_VENDOR_ID_BUSLOGIC:	return "BusLogic";
	      case PCI_VENDOR_ID_OAK: 		return "OAK";
	      case PCI_VENDOR_ID_PROMISE:	return "Promise Technology";
	      case PCI_VENDOR_ID_N9:		return "Number Nine";
	      case PCI_VENDOR_ID_UMC:		return "UMC";
	      case PCI_VENDOR_ID_X:		return "X TECHNOLOGY";
	      case PCI_VENDOR_ID_NEXGEN:	return "Nexgen";
	      case PCI_VENDOR_ID_QLOGIC:	return "Q Logic";
	      case PCI_VENDOR_ID_LEADTEK:	return "Leadtek Research";
	      case PCI_VENDOR_ID_CONTAQ:	return "Contaq";
	      case PCI_VENDOR_ID_FOREX:		return "Forex";
	      case PCI_VENDOR_ID_OLICOM:	return "Olicom";
	      case PCI_VENDOR_ID_CMD:		return "CMD";
	      case PCI_VENDOR_ID_VISION:	return "Vision";
	      case PCI_VENDOR_ID_SIERRA:	return "Sierra";
	      case PCI_VENDOR_ID_ACC:		return "ACC MICROELECTRONICS";
	      case PCI_VENDOR_ID_WINBOND:	return "Winbond";
	      case PCI_VENDOR_ID_3COM:		return "3Com";
	      case PCI_VENDOR_ID_AL:		return "Acer Labs";
	      case PCI_VENDOR_ID_ASP:		return "Advanced System Products";
	      case PCI_VENDOR_ID_CERN:		return "CERN";
	      case PCI_VENDOR_ID_IMS:		return "IMS";
	      case PCI_VENDOR_ID_TEKRAM2:	return "Tekram";
	      case PCI_VENDOR_ID_AMCC:		return "AMCC";
	      case PCI_VENDOR_ID_INTERG:	return "Intergraphics";
	      case PCI_VENDOR_ID_REALTEK:	return "Realtek";
	      case PCI_VENDOR_ID_INIT:		return "Initio Corp";
	      case PCI_VENDOR_ID_VIA:		return "VIA Technologies";
	      case PCI_VENDOR_ID_VORTEX:	return "VORTEX";
	      case PCI_VENDOR_ID_EF:		return "Efficient Networks";
	      case PCI_VENDOR_ID_FORE:		return "Fore Systems";
	      case PCI_VENDOR_ID_IMAGINGTECH:	return "Imaging Technology";
	      case PCI_VENDOR_ID_PLX:		return "PLX";
	      case PCI_VENDOR_ID_ALLIANCE:	return "Alliance";
	      case PCI_VENDOR_ID_VMIC:		return "VMIC";
	      case PCI_VENDOR_ID_DIGI:		return "Digi Intl.";
	      case PCI_VENDOR_ID_MUTECH:	return "Mutech";
	      case PCI_VENDOR_ID_TOSHIBA:	return "Toshiba";
	      case PCI_VENDOR_ID_ZEITNET:	return "ZeitNet";
	      case PCI_VENDOR_ID_SPECIALIX:	return "Specialix";
	      case PCI_VENDOR_ID_RP:		return "Comtrol";
	      case PCI_VENDOR_ID_CYCLADES:	return "Cyclades";
	      case PCI_VENDOR_ID_SYMPHONY:	return "Symphony";
	      case PCI_VENDOR_ID_TEKRAM:	return "Tekram";
	      case PCI_VENDOR_ID_3DLABS:	return "3Dlabs";
	      case PCI_VENDOR_ID_AVANCE:	return "Avance";
	      case PCI_VENDOR_ID_S3:		return "S3 Inc.";
	      case PCI_VENDOR_ID_INTEL:		return "Intel";
	      case PCI_VENDOR_ID_ADAPTEC:	return "Adaptec";
	      case PCI_VENDOR_ID_ATRONICS:	return "Atronics";
	      case PCI_VENDOR_ID_HER:		return "Hercules";
	      default:				return "Unknown vendor";
	}
}

/***************************************************************************/



/* shows the detected devices */
void pci_show(void)
{
    int i;
    struct pci_regs *r;

    printk("        PCI config type %i - %i PCI devices found\n", PCItype,ndev);

    for(i = 0; i < ndev; i++) {
	    printk("%i: bus %i dev %i\n",i,pci_devs[i].bus, pci_devs[i].dev);
	    r = (struct pci_regs *) pci_devs[i].mem;
	    printk("        Vendor: %s", pci_strvendor(r->VendorId));
	    printk("        Class: %s\n", pci_strclass(r->ClassCode << 8));
    }
}

/*Linux-->unsigned long pci_init(unsigned long mem_start, unsigned long mem_end) */
/*	H-PCI -->	void pci_init(void)				*/
/*	(mem_start/mem_end have no sense)				*/

int pci_init(void)
{
    if (pcibios_init() == -1) {
	return -1;
    }
    ndev = pci_scan_bus(pci_devs);
    return 1;
}
     
