USE.LIBS= VUTILS-THREADED ICF2-THREADED SDK VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=	\
	image.o \
	generators.o \
	syntheticStub.o

SRC=	\
	image.cc \
	generators.cc \
	syntheticStub.cc

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
