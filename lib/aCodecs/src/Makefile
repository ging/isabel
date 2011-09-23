USE.LIBS= ACODECS FFMPEG
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=codecs

TARGET=libacodecs.so 

aCodecs.Version:=$(shell cat ../version)

CXXFLAGS+= -I.

CXXFLAGS+=-DVAD=VAD1 -DMODE=NORM -DWMOPS=0 -DMMS_IO #needed by amr codec
#CXXFLAGS+=-DWAV49

CODECS=g711 g722 g726 g728 none

SRC= codecs.cxx             \
     soundutils.cxx         \
     codecs/ffmpegWrapper.cxx \
     codecs/noneCodec.cxx   \
     codecs/g711Codec.cxx   \
     codecs/g722Codec.cxx   \
     codecs/g726Codec.cxx   \
     codecs/g728Codec.cxx   \
     codecs/gsmCodec.cxx    \
     codecs/mp3Codec.cxx    \
     codecs/speexCodec.cxx  \
     codecs/amrnbCodec.cxx

OBJ=$(SRC:%.cxx=$(OBJDIR)/%.o)

EXTRA_LIBS=-lgsm -lspeex -lopencore-amrnb $(OBJDIR)/codecs/mp3/libmp3.a

files=$(foreach dir,$(CODECS),$(wildcard codecs/$(dir)/*.c*))
OBJ1=$(files:%.cxx=$(OBJDIR)/%.o) 
OBJ2=$(OBJ1:%.c=$(OBJDIR)/%.o)
OBJ3=$(OBJ2:%.cpp=$(OBJDIR)/%.o)

$(OBJDIR)/libacodecs.so: $(OBJ) $(OBJ3)
	$(LD.SHARED) -o $@.$(aCodecs.Version) $(OBJ) $(OBJ3) $(LINK.FFMPEG) $(EXTRA_LIBS) -Xlinker -soname=$$(basename $@).$(aCodecs.Version)
	-rm -f $@
	ln -s ./libacodecs.so.$(aCodecs.Version) $@

$(OBJDIR)/libacodecs.a: $(OBJ) $(OBJ3)
	ar cr $@ $(OBJ) $(OBJ3)

distrib::
	@echo "libacodecs.so -> $(DISTRIB_DIR)/isabelcore/lib"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@cp -f $(OBJDIR)/libacodecs.so.* $(DISTRIB_DIR)/isabelcore/lib
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/lib/libacodecs.so.*
	@chmod 644 $(DISTRIB_DIR)/isabelcore/lib/libacodecs.so.*
	@ln -sf libacodecs.so.$(aCodecs.Version)  $(DISTRIB_DIR)/isabelcore/lib/libacodecs.so

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
