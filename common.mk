
C_OBJS = $(C_SRC:.c=.o)
S_OBJS= $(S_SRC:.S=.o)

OBJS+=$(ARCH_SPEC_OBJS) $(C_OBJS) $(S_OBJS)
CLEAN_FILES += $(OBJS) .dep

INC+=-I$(ROOT)/drv
INC+=-I$(ROOT)/hal/$(CPU)/$(BOARD)/drv
INC+=-I$(ROOT)/include
INC+=-I$(ROOT)/hal/$(CPU)/$(BOARD)/include

clean:
	rm -r $(TARGET_LIB) $(CLEAN_FILES) 
	
build_lib: dep $(OBJS) makefile
	ar rcs $(TARGET_LIB) $(OBJS)
	ranlib $(TARGET_LIB)
	
build_rel: 	dep $(OBJS) makefile
	$(LD) -o $(TARGET_LIB) -r $(OBJS) 
		
	
dep: makefile $(S_OBJS:.o=.s) $(C_OBJS:.o=.c)
	$(GCC) -M $(CC_FLAGS) $(INC) $(S_OBJS:.o=.S) $(C_OBJS:.o=.c) > .dep 
	

# implicit rules
%.o:%.c
	$(GCC) $(CC_FLAGS) $(INC) -c -o $@ $<
%.o:%.S
	$(AS) -c -o $@ $<
