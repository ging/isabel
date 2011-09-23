USE.LIBS= VUTILS-THREADED ICF2-THREADED SDK VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=			\
	$(OBJDIR)/pnmLoad.o

SRC=	\
	pnmLoad.cc

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)

clean::
	rm -f $(OBJ)

cleanall:: clean

include $(PROJECT_ROOT_DIR)/Makefile.tail
