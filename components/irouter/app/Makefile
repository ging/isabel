USE.LIBS= RTP-NON-THREADED VUTILS-NON-THREADED ICF2-NON-THREADED STD VCODECS ACODECS
include $(PROJECT_ROOT_DIR)/Makefile.head


TARGET=irouter

SRC=    \
	main.cc transApp.cc \
	ctrlProtocol.cc centralSwitch.cc \
	clientModule.cc flow.cc output.cc \
	RTPFecHolder.cc \
	link.cc pkt.cc linkProtocol.cc \
	irouterGatherer.cc irouterStats.cc \
	configFile.cc configParam.cc \
	SoundUtils.cc \
	videoTrans.cc \
	random32.cc measures.cc \
        AudioMixer2.cc mixerbuffer.cc 

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)

$(OBJDIR)/irouter: $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LINK.RTP) $(LINK.VUTILS) $(LINK.ICF) $(LINK.STD) $(LINK.VCODECS) $(LINK.ACODECS)

distrib::
	@echo "isabel_irouter --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp $(OBJDIR)/irouter $(DISTRIB_DIR)/isabelcore/bin/isabel_irouter
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_irouter
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_irouter

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
