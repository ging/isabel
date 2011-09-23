USE.LIBS= VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET= CellBImpl.o

SRC=	CellBImpl.cxx

OBJ=$(SRC:%.cxx=$(OBJDIR)/%.o)


clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
