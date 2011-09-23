USE.LIBS= ACODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET= funccf.o decod.o codif.o opercf.o

SRC= funccf.cxx decod.cxx codif.cxx opercf.cxx

OBJ=$(SRC:%.cxx=$(OBJDIR)/%.o)

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
