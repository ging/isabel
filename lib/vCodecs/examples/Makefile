USE.LIBS= VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

CXXFLAGS+=-I../include -ggdb

TARGET = test 

SOURCES = test.cxx 

OBJ=$(SOURCES:%.cxx=$(OBJDIR)/%.o)

test: $(OBJ)
	$(LD) -o $@ $(OBJ) $(LINK.VCODECS)
#	$(CXX) -o $@ $(OBJ) -lvcodecs -lxvidcore -lH263 -lh264enc -lh264dec -lvcodecutil -lJPEG -ljpeg


clean::
	rm -f $(OBJ) 

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
















