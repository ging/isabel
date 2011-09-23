USE.LIBS= RTP-NON-THREADED ICF2-NON-THREADED STD ACODECS VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE= gridComposer

TARGET=mcu

SRC=      \
    audioTrans.cc \
    demux.cc \
    flowProcessor.cc \
    grid.cc \
    grid2.cc \
    mixer.cc \
    RTCPRecv.cc \
    sessionManager.cc \
    soundUtils.cc \
    traffic.cc\
    controlProtocol.cc \
    fec.cc \
    keepAlive.cc \
    output.cc \
    socketPool.cc \
    statGatherer.cc \
    videoTrans.cc\
    filter.cc \
    general.cc \
    main.cc \
    returnCode.cc \
    sender.cc \
    soundCodecs.cc \
    switch.cc \
    switchSSRC.cc \
    videoCodecs.cc \
    vuMeter.cc\
    gridComposer/gridAuto.cc \
    gridComposer/gridComposer.cc \
    gridComposer/gridBorder1.cc \
    gridComposer/gridBorderTR.cc \
    gridComposer/gridPip.cc


OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)

$(OBJDIR)/mcu: $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LINK.RTP) $(LINK.ICF) $(LINK.STD) $(LINK.ACODECS) $(LINK.VCODECS)

distrib::
	@echo "MCU --> $(DISTRIB_DIR)/bin"
	@cp $(OBJDIR)/mcu $(DISTRIB_DIR)/bin/isabel_mcu
	@$(STRIP) $(DISTRIB_DIR)/bin/isabel_mcu
	@chmod a+x $(DISTRIB_DIR)/bin/isabel_mcu

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
