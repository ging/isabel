USE.LIBS= VUTILS-THREADED ICF2-THREADED SDK X11 THREADS VCODECS ACODECS FFMPEG
include $(PROJECT_ROOT_DIR)/Makefile.head

ifeq ($(TARGET.OS),Linux)
TARGET= libicfsdk.so
endif
ifeq ($(TARGET.OS),SunOS)
TARGET= libicfsdk.so
endif

SUBDIRS.PRE= sources gif bmp png tiff pnm jpeg
SUBDIRS.POST=

PNG_LDFLAGS=$(shell libpng12-config --static --ldflags)
FMTS_LIB=-ltiff -ljpeg $(PNG_LDFLAGS)

ifeq ($(HAVE_LIBV4L),yes)
V4L_LIB_PATH=/usr/lib
ifeq ($(LINUX.DISTRIB.CODENAME),oneiric)
V4L_LIB_PATH= /usr/lib/i386-linux-gnu
endif
FMTS_LIB+= $(V4L_LIB_PATH)/libv4l2.a $(V4L_LIB_PATH)/libv4lconvert.a -lrt
endif

# Version number need to be changed whenever:
#
#   i) New virtual methods are added to current classes
#  ii) New attributes are added to current classes
#
# It is adequated (but not mandatory) whenever: 
#
#    i) New classes are added
#   ii) New (non virtual) methods are added to current classes
#  iii) New 'static' are added
# 
# It is foolish to do it whenever:
# 
#   i) A method implementation is changed
#  ii) Forgotten traces are removed :)

SDK.Version:=$(shell cat ../version)


SRC_SDK=	\
	imageLoader.cc		\
	systemRegistry.cc

SRC_FORMATS=	\
	gif/gifEnc.cc	\
	gif/gifDec.cc	\
	bmp/bmpEnc.cc	\
	bmp/bmpDec.cc	\
	png/pngEnc.cc	\
	png/pngDec.cc	\
	tiff/tiffDec.cc	\
	pnm/pnmLoad.cc	\
	jpeg/jpegImpl.cc	\
	jpeg/jpegmgr.cc		\
	jpeg/jpegtables.cc

SOURCES= Synthetic v4lUnified shmGrabber fileReader/aviread fileReader winGrabber urlGrabber
SRC_SOURCES=	\
	sources/fileReader/aviReader.cc            \
	sources/fileReader/fileReaderStub.cc            \
	sources/fileReader/imageSource.cc            \
            	\
	sources/fileReader/aviread/AviReadHandler.cpp            \
	sources/fileReader/aviread/AviReadStream.cpp            \
	sources/fileReader/aviread/avm_output.cpp            \
	sources/fileReader/aviread/BaseError.cpp            \
	sources/fileReader/aviread/Cache.cpp            \
	sources/fileReader/aviread/fcc_type.cpp            \
	sources/fileReader/aviread/Locker.cpp            \
	sources/fileReader/aviread/ReadStream.cpp            \
	sources/fileReader/aviread/StreamInfo.cpp            \
	sources/fileReader/aviread/utils.cpp            \
	sources/fileReader/aviread/wave_type.cpp            \
            	\
	sources/shmGrabber/shmGrabber.cc            \
	sources/shmGrabber/shmStub.cc            \
            	\
	sources/Synthetic/generators.cc            \
	sources/Synthetic/image.cc            \
	sources/Synthetic/syntheticStub.cc            \
            	\
	sources/urlGrabber/ffWrapper.cc            \
	sources/urlGrabber/imageBuf.cc            \
	sources/urlGrabber/urlGrabber.cc            \
	sources/urlGrabber/urlStub.cc            \
            	\
	sources/v4lUnified/v4l2DS.cc            \
	sources/v4lUnified/v4luDioctl.cc            \
	sources/v4lUnified/v4luGrabber.cc            \
	sources/v4lUnified/v4luStub.cc            \
	sources/v4lUnified/VideoHandler.cc            \
            	\
	sources/winGrabber/winGrabber.cc            \
	sources/winGrabber/winGrabberStub.cc            \
	sources/winGrabber/xvgrab.cc

ifeq ($(HAVE_V4L1),yes)
SRC_SOURCES+= sources/v4lUnified/v4l1DS.cc
endif

SRC=$(SRC_SDK) $(SRC_FORMATS) $(SRC_SOURCES)

OBJ1=$(SRC:%.c=$(OBJDIR)/%.o)
OBJ2=$(OBJ1:%.cc=$(OBJDIR)/%.o)
OBJ3=$(OBJ2:%.cxx=$(OBJDIR)/%.o)
OBJ=$(OBJ3:%.cpp=$(OBJDIR)/%.o)


$(OBJDIR)/libicfsdk.a: $(OBJ)
	ar cr $@ $?

$(OBJDIR)/libicfsdk.so: $(OBJ)
	$(LD.SHARED) -o $@.$(SDK.Version) $(OBJ) $(LINK.ICF) $(LINK.X11) -lXext -lXmu $(LINK.THREADS) $(LINK.STD) $(LINK.ACODECS) $(LINK.VCODECS) $(LINK.FFMPEG) $(FMTS_LIB) -Xlinker -soname=$$(basename $@).$(SDK.Version)
	-rm -f $@
	ln -s ./libicfsdk.so.$(SDK.Version) $@

distrib::
	@echo "libicfsdk.so -> $(DISTRIB_DIR)/isabelcore/lib"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@cp -f $(OBJDIR)/libicfsdk.so.* $(DISTRIB_DIR)/isabelcore/lib
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/lib/libicfsdk.so.*
	@chmod 644 $(DISTRIB_DIR)/isabelcore/lib/libicfsdk.so.*
	@ln -sf libicfsdk.so.$(SDK.Version) $(DISTRIB_DIR)/isabelcore/lib/libicfsdk.so

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)
	rm -f $(OBJDIR)/libicfsdk.so*

include $(PROJECT_ROOT_DIR)/Makefile.tail
