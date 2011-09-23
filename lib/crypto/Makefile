include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=libkryptonite.a 

CXXFLAGS += -x c

SRC=\
	cbc_cksum.c cbc_encrypt.c \
	ecb_encrypt.c pcbc_encrypt.c \
	quad_cksum.c random_key.c \
	set_key.c string2key.c \
	enc_read.c enc_write.c fcrypt.c

OBJ=$(SRC:%.c=$(OBJDIR)/%.o)

$(OBJDIR)/libkryptonite.a: $(OBJ)
	ar cr $@ $?

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
