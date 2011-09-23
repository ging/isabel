USE.LIBS= ACODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET= g726_24Imp.o g726_32Imp.o g72x.o g726_16Imp.o g726_40Imp.o  

SRC= g726_24Imp.cxx g726_32Imp.cxx g72x.cxx g726_16Imp.cxx g726_40Imp.cxx  

OBJ=$(SRC:%.cxx=$(OBJDIR)/%.o)

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
