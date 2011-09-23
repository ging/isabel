USE.LIBS= VUTILS-THREADED ICF2-THREADED SDK VCODECS FFMPEG
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=	\
	imageBuf.o \
	ffWrapper.o \
	urlGrabber.o \
	urlStub.o

SRC=	\
	imageBuf.cc	\
	ffWrapper.cc	\
	urlGrabber.cc	\
	urlStub.cc 

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
