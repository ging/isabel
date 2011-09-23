USE.LIBS= VCODECS FFMPEG
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=codecs

TARGET=libvcodecs.so 

vCodecs.Version:=$(shell cat ../version)

OWN_CODECS= CellB H263 JPEG

SRC= codec.cxx \
     codecs.cxx \
     colnames.cxx \
     smooth.cxx \
     vUtils.cxx \
     colorspace.cxx \
     codecs/ffmpegWrapper.cxx \
     codecs/CellBCodec.cxx \
     codecs/H263Codec.cxx \
     codecs/XVIDCodec.cxx \
     codecs/MJPEGCodec.cxx \
     codecs/H264Codec.cxx

OBJN=$(SRC:%.cxx=$(OBJDIR)/%.o)
OBJ=$(OBJN:%.c=$(OBJDIR)/%.o)

EXTRA_LIBS=-ljpeg -lx264 -lxvidcore

files=$(foreach dir,$(OWN_CODECS),$(wildcard codecs/$(dir)/*.c*))
OBJ1=$(files:%.cxx=$(OBJDIR)/%.o) 
OBJ2=$(OBJ1:%.c=$(OBJDIR)/%.o)
OBJ3=$(OBJ2:%.cc=$(OBJDIR)/%.o)

$(OBJDIR)/libvcodecs.so: $(OBJ) $(OBJ3)
	$(LD.SHARED) -o $@.$(vCodecs.Version) $(OBJ) $(OBJ3) $(LINK.FFMPEG) $(EXTRA_LIBS) -Xlinker -soname=$$(basename $@).$(vCodecs.Version)
	-rm -f $@
	ln -s ./libvcodecs.so.$(vCodecs.Version) $@

distrib::
	@echo "libvcodecs.so -> $(DISTRIB_DIR)/isabelcore/lib"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@cp -f $(OBJDIR)/libvcodecs.so.* $(DISTRIB_DIR)/isabelcore/lib
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/lib/libvcodecs.so.*
	@chmod 644 $(DISTRIB_DIR)/isabelcore/lib/libvcodecs.so.*
	@ln -sf libvcodecs.so.$(vCodecs.Version)  $(DISTRIB_DIR)/isabelcore/lib/libvcodecs.so

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
