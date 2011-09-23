USE.LIBS= VUTILS-THREADED ICF2-THREADED SDK VCODECS ACODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE= aviread
SUBDIRS.POST=

TARGET=	fileReaderStub.o imageSource.o aviReader.o

SRC=	fileReaderStub.cc imageSource.cc aviReader.cc

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
