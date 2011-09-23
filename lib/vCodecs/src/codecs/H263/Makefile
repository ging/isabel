USE.LIBS= VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=	\
	chendct.o \
	cod.o \
	dec.o \
	escritor.o \
	H263Encoder.o \
	H263Impl.o \
	lector.o \
	tablas.o

SRC=	\
	chendct.cxx \
	cod.cxx \
	dec.cxx \
	escritor.cxx \
	H263Encoder.cxx \
	H263Impl.cxx \
	lector.cxx \
	tablas.cxx

OBJ=$(SRC:%.cxx=$(OBJDIR)/%.o)


clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
