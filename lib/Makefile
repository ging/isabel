include $(PROJECT_ROOT_DIR)/Makefile.head

ifeq ($(TARGET.OS),Win32)
SUBDIRS.PRE=aCodecs vCodecs ICF2 RTP vUtils
SUBDIRS.POST=tests
endif

ifeq ($(TARGET.OS),Linux)
SUBDIRS.PRE=jars ffmpeg aCodecs vCodecs ICF2 RTP vUtils SDK GWR
SUBDIRS.POST=tclcommon javacommon tests
endif

ifeq ($(TARGET.OS),Darwin)
SUBDIRS.PRE=jars ffmpeg aCodecs vCodecs ICF2 RTP vUtils SDK GWR
SUBDIRS.POST=tclcommon javacommon tests
endif

ifeq ($(TARGET.OS),SunOS)
SUBDIRS.PRE=jars ffmpeg aCodecs vCodecs ICF2 RTP vUtils SDK GWR
SUBDIRS.POST=tclcommon javacommon tests
endif


include $(PROJECT_ROOT_DIR)/Makefile.tail
