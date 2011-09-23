USE.LIBS= SDK VUTILS-THREADED RTP-THREADED ICF2-THREADED THREADS ACODECS VCODECS GWR
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

#CXX=insure
#LD=insure

ifeq ($(TARGET.OS),Linux)
ifeq ($(LINUX.DISTRIB.ID),Ubuntu)
ifeq ($(LINUX.DISTRIB.CODENAME),intrepid)
CXXFLAGS+=-DMPEG4_NOT_AVAILABLE
endif
ifeq ($(LINUX.DISTRIB.CODENAME),jaunty)
CXXFLAGS+=-DMPEG4_NOT_AVAILABLE
endif
endif
endif


TARGET=video

SRC=	                \
	renderer.cc         \
	sourceTask.cc       \
	winSink.cc          \
	netSink.cc          \
	channel.cc          \
	playchannel.cc      \
	recchannel.cc       \
	netchannel.cc       \
	channelMgr.cc       \
	ctrlProtocol.cc     \
	videoApp.cc         \
	videoRTPSession.cc  \
	videoConfig.cc      \
	stats.cc            \
	extRTPSession.cc    \
	main.cc

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


$(OBJDIR)/video: $(OBJ)
	$(LD) -o $@ $(OBJ) $(LINK.SDK) $(LINK.VUTILS) $(LINK.ICF) $(LINK.RTP) $(LINK.THREADS) $(LINK.ACODECS) $(LINK.VCODECS) $(LINK.GWR)

distrib::
	@echo "isabel_video --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp -f $(OBJDIR)/video $(DISTRIB_DIR)/isabelcore/bin/isabel_video
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_video
	@chmod 555 $(DISTRIB_DIR)/isabelcore/bin/isabel_video

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
