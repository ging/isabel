USE.LIBS= VUTILS-THREADED ICF2-THREADED SDK VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=	\
	v4l2DS.o		\
	v4luDioctl.o		\
	v4luGrabber.o		\
        VideoHandler.o		\
	v4luStub.o

SRC=				\
        v4l2DS.cc		\
	v4luDioctl.cc		\
	v4luGrabber.cc		\
        VideoHandler.cc		\
	v4luStub.cc

ifeq ($(HAVE_V4L1),yes)
TARGET += v4l1DS.o

SRC+= v4l1DS.cc
endif

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
