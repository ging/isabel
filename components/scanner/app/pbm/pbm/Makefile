CONFIG.generic.define+=-I../

include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=libpbm.a

CXXFLAGS += -x c

SRC=\
	libpbm1.c \
	libpbm2.c \
	libpbm3.c \
	libpbm4.c \
	libpbm5.c

OBJ=$(SRC:%.c=$(OBJDIR)/%.o)



$(OBJDIR)/libpbm.a: $(OBJ)
	ar cr $@ $?

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
