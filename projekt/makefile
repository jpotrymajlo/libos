
.SUFFIXES: .c .s

TARGET_LIB=libappl.a


ARCH_SPEC_SRC=
ARCH_SPEC_OBJS = $(ARCH_SPEC_SRC:.c=.o)

C_SRC = task.c 


include $(ROOT)/common.mk

# targets

build:	build_lib
-include .dep	


