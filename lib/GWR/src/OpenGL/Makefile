USE.LIBS= ICF2-THREADED X11 THREADS VCODECS GWR 
include $(PROJECT_ROOT_DIR)/Makefile.head

CXXFLAGS += -DHAVE_OPENGL

TARGET= \
        $(OBJDIR)/dtOpenGL.o

SRC=\
        dtOpenGL.cc

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


distrib::

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

