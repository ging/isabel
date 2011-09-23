USE.LIBS= ICF2-NON-THREADED
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET= alsasounddevice.o

SRC= alsasounddevice.cpp

OBJ=$(SRC:%.cpp=$(OBJDIR)/%.o)


distrib::

clean::
	rm -f $(OBJ)

cleanall:: clean

include $(PROJECT_ROOT_DIR)/Makefile.tail
