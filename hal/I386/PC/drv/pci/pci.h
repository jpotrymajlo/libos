/* Project:     HARTIK 3.0 Network Library                      */
/* Description: Hard Real TIme Kernel for 8086 compatible       */
/* Author:      Luca Abeni					*/
/* Date:        14/4/1997                                       */
                           
/* File:        PCI.H                                           */
/* Revision:    1.02                                            */

#ifndef __PCI_H__
#define __PCI_H__

#ifdef __cplusplus
extern "C" {
#endif

#define N_MAX_DEVS 10

typedef struct pci_regs {
	WORD VendorId;
	WORD DeviceId;
	WORD PciCommand;
	WORD PciStatus;
	BYTE reserved1;
	BYTE dummy;
	WORD ClassCode;
	BYTE reserved2;
	BYTE LatencyTimer;
	BYTE HeaderType;
	BYTE reserved3;
	DWORD IoBaseAddress;
	DWORD reserved4;
	DWORD reserved5;
	DWORD reserved6;
	DWORD reserved7;
	DWORD reserved8;
	DWORD reserved9;
	DWORD reserved10;
	DWORD BiosRomControl;
	DWORD reserved11;
	DWORD reserver12;
	BYTE InterruptLine;
	BYTE InterruptPin;
	BYTE MinGnt;
	BYTE MaxLat;
	DWORD InternalConfig;
} PCI_REGS;

typedef struct pci_des {
		BYTE bus;
		BYTE dev;
		BYTE mem[256];
} PCI_DES;


/*int pci_detect(struct pci_des *p_des);
void pci_show(struct pci_des *PCIdes, int n);*/
int pci_init(void);
void pci_show(void);
int scan_bus(struct pci_des *p_des);
struct pci_regs *pci_find_class(DWORD class_code, BYTE *bus, BYTE *dev);
struct pci_regs *pci_find_device(WORD vendor, WORD device, BYTE *bus, BYTE *dev);

extern int (*pcibios_read_config_byte)(BYTE bus, BYTE dev, BYTE where, BYTE *val);
extern int (*pcibios_read_config_word)(BYTE bus, BYTE dev, BYTE where, WORD *val);
extern int (*pcibios_read_config_dword)(BYTE bus, BYTE dev, BYTE where, DWORD *val);
extern int (*pcibios_write_config_byte)(BYTE bus, BYTE dev, BYTE where, BYTE val);
extern int (*pcibios_write_config_word)(BYTE bus, BYTE dev, BYTE where, WORD val);
extern int (*pcibios_write_config_dword)(BYTE bus, BYTE dev, BYTE where, DWORD val);
#ifdef __cplusplus
};
#endif

#endif

