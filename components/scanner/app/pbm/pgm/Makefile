CONFIG.generic.include+=-I../pbm -I../

include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=libpgm.a

CXXFLAGS += -x c

SRC=\
	libpgm1.c \
	libpgm2.c

OBJ=$(SRC:%.c=$(OBJDIR)/%.o)

$(OBJDIR)/libpgm.a: $(OBJ)
	ar cr $@ $?

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
