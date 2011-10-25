
.SUFFIXES: .s

TARGET_LIB=libarch.a



C_SRC= pxa.c  \
       uart.c \
       mmu.c  \
       start.c \
       cache.c
       
S_SRC=funcs.s \
      intr.s
      
INC+=-I$(ROOT)/hal/$(CPU)/$(BOARD)/include



include $(ROOT)/common.mk

# targets
build:	build_lib 


-include .dep
