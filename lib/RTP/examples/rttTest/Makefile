include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=rttTest

SRC=rttTest.cc        



OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


$(OBJDIR)/rttTest: $(OBJ)
	 $(CXX) -o$@ $(OBJ) $(LINK.ICF) $(LINK.RTP) $(LINK.THREADS)

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
