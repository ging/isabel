USE.LIBS= RTP-NON-THREADED ICF2-NON-THREADED THREADS ACODECS FFMPEG
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET= \
	ringBuffer.o \
	audioapp.o \
	channel.o \
	recchannel.o \
	channelmgr.o \
	fileReader.o \
	toneGenerator.o \
	playchannel.o \
	mixerbuffer.o \
	vumeter.o \
	statssender.o \
	rtpsink.o \
	udpsound.o \
	packetqueue.o

SRC=	\
	ringBuffer.cpp \
	audioapp.cpp \
	ctrlprotocol.cpp \
	channel.cpp \
	recchannel.cpp \
	channelmgr.cpp \
	fileReader.cpp \
	toneGenerator.cpp \
	playchannel.cpp \
	mixerbuffer.cpp \
	vumeter.cpp \
	statssender.cpp \
	rtpsink.cpp \
	udpsound.cpp \
	packetqueue.cpp

OBJ=$(SRC:%.cpp=$(OBJDIR)/%.o)


distrib::

clean::
	rm -f $(OBJ)

cleanall:: clean


include $(PROJECT_ROOT_DIR)/Makefile.tail
