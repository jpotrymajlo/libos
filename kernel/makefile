
.SUFFIXES: .c .s

TARGET_LIB=libos.a


C_SRC = queue.c          \
	drv.c            \
	tsk.c            \
        msg.c            \
        sem.c            \
        sched.c          \
        init.c           \
	alloc.c          \
        errno.c               


include $(ROOT)/common.mk

# targets



build:	build_lib
	
-include .dep

