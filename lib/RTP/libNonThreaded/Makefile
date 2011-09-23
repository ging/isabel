USE.LIBS= ICF2-NON-THREADED RTP-NON-THREADED VCODECS ACODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=libnonthreadedicfrtp.a libnonthreadedicfrtp.so

FILES=\
       RTPPayloads.cc           \
       RTPSession.cc            \
       bindingHolder.cc         \
       RTPReceiver.cc           \
       RTCPReceiver.cc          \
       RTPSourcesReceived.cc    \
       RTPRandom.cc             \
       RTPSender.cc             \
       RTPStatisticsManager.cc  \
       RTPPacketProcessor.cc    \
       RTPPacket.cc             \
       RTPSource.cc             \
       RTPFec.cc                \
       jpegRFC2435.cc           \
       fragmenter.cc            \
       defragmenter.cc

SRC=$(FILES:%.cc=../src/%.cc)
OBJ=$(FILES:%.cc=$(OBJDIR)/%.o)

$(OBJDIR)/%.o:  ../src/%.cc
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

RTP.Version:=$(shell cat ../version)


$(OBJDIR)/libnonthreadedicfrtp.a: $(OBJ)
	ar cr $@ $?

$(OBJDIR)/libnonthreadedicfrtp.so: $(OBJ)
	$(LD.SHARED) -o $@.$(RTP.Version) $(OBJ) -Xlinker -soname=$$(basename $@).$(RTP.Version)
	-rm -f $@
	ln -s ./libnonthreadedicfrtp.so.$(RTP.Version) $@

distrib::
	@echo "libnonthreadedicfrtp.so --> $(DISTRIB_DIR)/isabelcore/lib"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@cp -f $(OBJDIR)/libnonthreadedicfrtp.so.* $(DISTRIB_DIR)/isabelcore/lib
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/lib/libnonthreadedicfrtp.so.*
	@ln -sf libnonthreadedicfrtp.so.$(RTP.Version) $(DISTRIB_DIR)/isabelcore/lib/libnonthreadedicfrtp.so

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)
	rm -f $(OBJDIR)/libnonthreadedicfrtp.so*

include $(PROJECT_ROOT_DIR)/Makefile.tail
