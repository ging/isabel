USE.LIBS= ICF2-NON-THREADED STD ACODECS VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head


TARGET=irouter
SAVE_MEASURES=-DIROUTER_SAVE_MEASURES
CXXFLAGS += $(SAVE_MEASURES)

SRC=main.cc transApp.cc ctrlProtocol.cc  \
    link.cc linkProtocol.cc \
    configFile.cc configParam.cc \
    cmdParser.cc mcuCommand.cc mcuSender.cc socketPool.cc 
OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


#
# $(LINK.ICF)     -- link icf
# $(LINK.SDK)     -- link sdk
# $(LINK.RTP)     -- link rtp
# $(LINK.ACODECS) -- link aCodecs
# $(LINK.X11)     -- link x11
# $(LINK.THREADS) -- pthreads (and Solaris threads) (required by icf)
# $(LINK.STD)     -- math, sockets, name services and the like
#


$(OBJDIR)/irouter: $(OBJ)
	$(CXX) -o$@ $(OBJ) $(LINK.ICF) $(LINK.STD) $(LINK.ACODECS) $(LINK.VCODECS)


distrib::
	@echo "isabel_irouter --> $(DISTRIB_DIR)/bin"
	@cp $(OBJDIR)/irouter $(DISTRIB_DIR)/bin/isabel_irouter
	@$(STRIP) $(DISTRIB_DIR)/bin/isabel_irouter
	@chmod a+x $(DISTRIB_DIR)/bin/isabel_irouter

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
