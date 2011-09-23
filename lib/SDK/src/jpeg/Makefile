USE.LIBS= VUTILS-THREADED ICF2-THREADED SDK VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=			\
	$(OBJDIR)/jpegImpl.o	\
	$(OBJDIR)/jpegtables.o	\
	$(OBJDIR)/jpegmgr.o



SRC=	\
	jpegImpl.cc	\
	jpegtables.cc	\
	jpegmgr.cc

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)

clean::
	rm -f $(OBJ)

cleanall:: clean

include $(PROJECT_ROOT_DIR)/Makefile.tail
