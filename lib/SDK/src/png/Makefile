USE.LIBS= VUTILS-THREADED ICF2-THREADED SDK VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

PNG_CXXFLAGS= $(shell libpng12-config --static --cflags)
CXX+=$(PNG_CXXFLAGS)

TARGET=			\
	$(OBJDIR)/pngDec.o	\
	$(OBJDIR)/pngEnc.o


SRC=	\
	pngDec.cc	\
	pngEnc.cc

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)

clean::
	rm -f $(OBJ)

cleanall:: clean

include $(PROJECT_ROOT_DIR)/Makefile.tail
