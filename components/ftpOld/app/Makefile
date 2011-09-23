USE.LIBS= RTP-THREADED ICF2-THREADED THREADS
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_cftp

SRC=fchannelMgr.cc ctrlProtocol.cc fileTrans.cc  \
    cftpApp.cc main.cc protMgr.cc cftpRTPSession.cc \

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


$(OBJDIR)/isabel_cftp: $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LINK.ICF) $(LINK.THREADS) $(LINK.STD) $(LINK.RTP)

distrib::
	@echo "isabel_cftp --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp $(OBJDIR)/isabel_cftp $(DISTRIB_DIR)/isabelcore/bin/
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_cftp
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_cftp

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
