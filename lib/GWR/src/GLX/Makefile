USE.LIBS= ICF2-THREADED X11 THREADS VCODECS GWR
include $(PROJECT_ROOT_DIR)/Makefile.head

CXXFLAGS += -DHAVE_X11

TARGET= \
        $(OBJDIR)/dtGLX.o

SRC=\
        dtGLX.cc

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


distrib::

clean::
	rm -f $(OBJ)

cleanall:: clean

include $(PROJECT_ROOT_DIR)/Makefile.tail

