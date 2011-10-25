.SUFFIXES: .asm

# defines
# system version
VERSION := $(shell pwd)
export ROOT := $(shell pwd)


export CPU   ?=I386
export BOARD ?=PC


#sciezki
export TARGET_FILE= kernel

LIB_ARCH_DIR     = hal/$(CPU)/$(BOARD)/arch
LIB_ENTRY_DIR    = hal/$(CPU)/$(BOARD)/entry
LIB_SHELL_DIR    = sh
LIB_DRV_DIR      = drv
LIB_DRV_ARCH_DIR = hal/$(CPU)/$(BOARD)/drv
LIB_C_DIR        = libc
LIB_KERNEL_DIR   = kernel
LIB_APP_DIR      = projekt

ENTRY_FILE   = $(LIB_ENTRY_DIR)/entry.o
LIB_ARCH     = $(LIB_ARCH_DIR)/libarch.a
LIB_APP      = $(LIB_APP_DIR)/libappl.a
LIB_SHELL    = $(LIB_SHELL_DIR)/libsh.a
LIB_C        = $(LIB_C_DIR)/libc.a
LIB_KERNEL   = $(LIB_KERNEL_DIR)/libos.a
LIB_DRV      = $(LIB_DRV_DIR)/libdrv.a
LIB_DRV_ARCH = $(LIB_DRV_ARCH_DIR)/libdrvarch.a

include hal/$(CPU)/$(BOARD)/scripts/makefile.arch

SUBDIRS=$(LIB_ENTRY_DIR)  \
        $(LIB_ARCH_DIR)   \
        $(LIB_SHELL_DIR)  \
        $(LIB_APP_DIR)    \
        $(LIB_KERNEL_DIR) \
        $(LIB_C_DIR)      \
        $(LIB_DRV_DIR)    \
        $(LIB_DRV_ARCH_DIR)
        
        
LIBS=$(LIB_DRV)    \
     $(LIB_DRV_ARCH) \
     $(LIB_APP)    \
     $(LIB_SHELL)  \
     $(LIB_C)      \
     $(LIB_KERNEL) \
     $(LIB_ARCH)

all: build install

# targets
build_subdirs:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir build; \
	done


build: build_subdirs
	$(LD) $(LD_FLAGS) -o $(TARGET_FILE).elf $(ENTRY_FILE) --start-group $(LIBS) --end-group
	$(STRIP) $(TARGET_FILE).elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) $(TARGET_FILE).elf zImage
	$(OBJDUMP) $(OBJDUMP_FLAGS) $(TARGET_FILE).elf > $(TARGET_FILE).dump


clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
	rm kernel.* zImage              


rebuild: clean build

doc:
	doxygen Doxyfile
	
install:
	cp zImage ~/tftpboot/	
#install:
#	sudo mount -o loop,umask=0 floppy.bin floppy
#	cp kernel.elf floppy/kernel.bin
#	sudo umount floppy

check:
	./utils/mbchk kernel.elf

run:  install
	qemu -fda floppy.bin -boot a
	






