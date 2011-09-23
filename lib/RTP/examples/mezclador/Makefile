include $(PROJECT_ROOT_DIR)/Makefile.head
 
TARGET=mezclador

SUBDIRS.PRE=
SUBDIRS.POST=


SRC=mezclador.cc
OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


#
# $(LINK.ICF)     -- link icf
# $(LINK.SDK)     -- link sdk
# $(LINK.RTP)     -- link rtp
# $(LINK.X11)     -- link x11
# $(LINK.THREADS) -- pthreads (and Solaris threads) (required by icf)
# $(LINK.STD)     -- math, sockets, name services and the like
#
$(OBJDIR)/mezclador: $(OBJ)
	$(CXX) -o$@ $(OBJ) $(LINK.ICF) $(LINK.THREADS) $(LINK.RTP) $(LINK.STD)

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

