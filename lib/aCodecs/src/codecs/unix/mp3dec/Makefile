USE.LIBS= ACODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

CXXFLAGS+= -x c -DHAVE_CONFIG_H -DUSE_LAYER_2 -DHAVE_MPGLIB

TARGET= \
	interface.o \
	mpglib_interface.o

SRC= \
	interface.cxx \
	mpglib_interface.cxx

OBJ=$(SRC:%.cxx=$(OBJDIR)/%.o) 

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
