include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=	\
	jpegmgr.o \
	mjpegImpl.o

SRC=	\
	jpegmgr.cc \
	mjpegImpl.cc

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
