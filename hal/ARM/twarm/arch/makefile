
#.SUFFIXES: .S

TARGET_LIB=libarch.a



C_SRC=$(wildcard *.c)
       
       
S_SRC=$(wildcard *.S)
      
INC+=-I$(ROOT)/hal/$(CPU)/$(BOARD)/include



include $(ROOT)/common.mk

# targets
build:	build_lib 


-include .dep
