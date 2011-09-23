USE.LIBS= RTP-NON-THREADED ICF2-NON-THREADED THREADS ACODECS FFMPEG
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_audio

SUBDIRS.PRE=common linux portaudio
SUBDIRS.POST=

CXXFLAGS+= -iquotelinux -iquotecommon


SRC= \
    common/ringBuffer.cpp \
    common/audioapp.cpp \
    common/ctrlprotocol.cpp \
    common/channel.cpp \
    common/recchannel.cpp \
    common/channelmgr.cpp \
    common/fileReader.cpp \
    common/toneGenerator.cpp \
    common/playchannel.cpp \
    common/mixerbuffer.cpp \
    common/vumeter.cpp \
    common/statssender.cpp \
    common/rtpsink.cpp \
    common/udpsound.cpp \
    common/packetqueue.cpp

ifeq ($(TARGET.OS),Linux)
SRC+= linux/alsasounddevice.cpp
SRC+= portaudio/pasounddevice.cpp
endif

OBJ=$(SRC:%.cpp=$(OBJDIR)/%.o)

$(OBJDIR)/$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LINK.RTP) $(LINK.ICF) $(LINK.ACODECS) $(LINK.FFMPEG) $(LINK.STD) -lportaudio -lasound

distrib::
	@echo "$(TARGET) --> $(DISTRIB_DIR)/isabelcore/bin/$(TARGET)"
	@cp -f $(OBJDIR)/$(TARGET) $(DISTRIB_DIR)/isabelcore/bin/$(TARGET)
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/$(TARGET)
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/$(TARGET)

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
