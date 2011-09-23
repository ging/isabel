USE.LIBS= ACODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=
TARGET=libmp3.a
CXXFLAGS+= -x c -DHAVE_CONFIG_H -DUSE_LAYER_2 -DHAVE_MPGLIB #needed by arm codec

SRC=$(wildcard *.c)
OBJ=$(SRC:%.c=$(OBJDIR)/%.o) 
$(OBJDIR)/libmp3.a: $(OBJ)  
	ar cr $@ $?

distrib::

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
