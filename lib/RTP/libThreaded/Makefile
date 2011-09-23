USE.LIBS= ICF2-THREADED RTP-THREADED VCODECS ACODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=libthreadedicfrtp.a libthreadedicfrtp.so

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


$(OBJDIR)/libthreadedicfrtp.a: $(OBJ)
	ar cr $@ $?

$(OBJDIR)/libthreadedicfrtp.so: $(OBJ)
	$(LD.SHARED) -o $@.$(RTP.Version) $(OBJ) -Xlinker -soname=$$(basename $@).$(RTP.Version)
	-rm -f $@
	ln -s ./libthreadedicfrtp.so.$(RTP.Version) $@

distrib::
	@echo "libthreadedicfrtp.so --> $(DISTRIB_DIR)/isabelcore/lib"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@cp -f $(OBJDIR)/libthreadedicfrtp.so.* $(DISTRIB_DIR)/isabelcore/lib
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/lib/libthreadedicfrtp.so.*
	@ln -sf libthreadedicfrtp.so.$(RTP.Version)  $(DISTRIB_DIR)/isabelcore/lib/libthreadedicfrtp.so

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)
	rm -f $(OBJDIR)/libthreadedicfrtp.so*

include $(PROJECT_ROOT_DIR)/Makefile.tail
