USE.LIBS= VUTILS-THREADED ICF2-THREADED SDK VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=			\
	$(OBJDIR)/bmpDec.o	\
	$(OBJDIR)/bmpEnc.o

SRC=			\
	bmpDec.cc	\
	bmpEnc.cc

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


clean::
	rm -f $(OBJ)

cleanall:: clean

include $(PROJECT_ROOT_DIR)/Makefile.tail
