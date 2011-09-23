include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=libavcodec

# FFMPEG general options
#     FFMPEG licensing
CONFIGURE_OPTS=  --enable-gpl --enable-version3
#     FFMPEG input devices
CONFIGURE_OPTS+= --enable-x11grab
#     FFMPEG required codecs
CONFIGURE_OPTS+= --enable-libgsm --enable-libspeex --enable-libmp3lame --enable-zlib
#     FFMPEG CPU and code generation options
CONFIGURE_OPTS+= --disable-stripping --enable-runtime-cpudetect --enable-postproc

ifeq ($(TARGET.OS),Darwin)
CONFIGURE_OPTS+= --disable-x264 --disable-mmx --enable-memalign-hack --disable-v4l --disable-v4l2
endif

ifeq ($(TARGET.OS),SunOS)
CONFIGURE_OPTS+= --disable-x264 --disable-mmx --disable-v4l --disable-v4l2
endif

$(OBJDIR)/libavcodec: ffmpeg/config.h ffmpeg/config.mak
	@echo "compiling ffmpeg"
	(cd ffmpeg; make -C libavformat all; make -C libavutil all; make -C libavcodec all)
	(cd ffmpeg; make ffmpeg)

ffmpeg/config.h: ffmpeg
	@echo "Configuring ffmpeg"
	(cd ffmpeg; ./configure $(CONFIGURE_OPTS))

ffmpeg/config.mak: ffmpeg
	@echo "Configuring ffmpeg"
	(cd ffmpeg; ./configure $(CONFIGURE_OPTS))

ffmpeg: ffmpeg-20110304.tar.bz2
	@echo "Removing symbolic link ffmpeg"
	rm -f ffmpeg
	@echo "Expanding ffmpeg from ffmpeg-20110304.tar.bz2"
	tar xvfj ffmpeg-20110304.tar.bz2
	@echo "Making symbolic link ffmpeg with ffmpeg-20110304.tar.bz2"
	ln -s ffmpeg-2011-03-04 ffmpeg

distrib::
	@echo "ffmpeg --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp -f ./ffmpeg/ffmpeg $(DISTRIB_DIR)/isabelcore/bin/isabel_ffmpeg
	@chmod 555 $(DISTRIB_DIR)/isabelcore/bin/isabel_ffmpeg

clean::
	@echo "Cleaning ffmpeg"
	@if [ -d ffmpeg ] ; then \
           cd ffmpeg ; make clean ;\
        fi

cleanall::
	@echo "Removing ffmpeg's expanded directory..."
	@rm -rf ffmpeg
	@rm -rf ffmpeg-2011-03-04

include $(PROJECT_ROOT_DIR)/Makefile.tail
