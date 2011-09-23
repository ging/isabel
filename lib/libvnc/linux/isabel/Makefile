include $(PROJECT_ROOT_DIR)/Makefile.head

CXXFLAGS+=-I../../include
TARGET=libvnc.a 
SRC_DIR= ../../src/
SRC= $(SRC_DIR)d3des.cpp  \
     $(SRC_DIR)image.cpp  \
     $(SRC_DIR)locker.cpp \
     $(SRC_DIR)tight.cpp  \
     $(SRC_DIR)vncconn.cpp\
     $(SRC_DIR)extras.cpp \
     $(SRC_DIR)jpeg.cpp   \
     $(SRC_DIR)rfb.cpp    \
     $(SRC_DIR)threadtask.cpp \
     $(SRC_DIR)vncauth.cpp

OBJ=$(SRC:%.cpp=$(OBJDIR)/%.o)

prev::
	mkdir -p $(OBJDIR)/$(SRC_DIR)

$(OBJDIR)/libvnc.a: prev $(OBJ)
	ar cr $@ $(OBJ)

distrib::

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
