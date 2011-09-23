USE.LIBS= VUTILS-THREADED ICF2-THREADED SDK VCODECS X11
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=	\
	winGrabberStub.o \
	winGrabber.o \
        xvgrab.o

SRC=	\
	winGrabberStub.cc \
	winGrabber.cc \
        xvgrab.cc

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)

clean::
	rm -f $(OBJXX)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
