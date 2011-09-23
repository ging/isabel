include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=rttTest

SUBDIRS.PRE=
SUBDIRS.POST=


SRC=rttTest.cc 
OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


#
# $(LINK.ICF)     -- link icf
# $(LINK.SDK)     -- link sdk
# $(LINK.X11)     -- link x11
# $(LINK.THREADS) -- pthreads (and Solaris threads) (required by icf)
# $(LINK.STD)     -- math, sockets, name services and the like
#
$(OBJDIR)/rttTest: $(OBJ)
	$(CXX) -o$@ $(OBJ) $(LINK.ICF) $(LINK.UCB) $(LINK.THREADS) \
                           $(LINK.STD) 

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
