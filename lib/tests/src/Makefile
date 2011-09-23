USE.LIBS= GWR SDK VUTILS-THREADED RTP-THREADED ICF2-THREADED THREADS X11 ACODECS VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

ifeq ($(TARGET.OS),Linux)
SUBDIRS.PRE=unix
SUBDIRS.POST=

TARGET=	\
	grabber_test		\
	format_test		\
	codec_test		\
	performance_test	\
	fragmenter_test		\
	image_loader_test
endif
ifeq ($(TARGET.OS),SunOS)
SUBDIRS.PRE=unix
SUBDIRS.POST=

TARGET=	\
	grabber_test		\
	format_test		\
	codec_test		\
	performance_test	\
	fragmenter_test		\
	image_loader_test
endif

$(OBJDIR)/grabber_test: $(OBJDIR)/grabberTest.o $(OBJDIR)/paint.o
	$(LD) -o $@ $^ $(LINK.SDK) $(LINK.VUTILS) $(LINK.RTP) $(LINK.GWR) $(LINK.ICF) $(LINK.THREADS) $(LINK.ACODECS) $(LINK.VCODECS)

$(OBJDIR)/format_test: $(OBJDIR)/formatTest.o $(OBJDIR)/paint.o
	$(LD) -o $@ $^ $(LINK.SDK) $(LINK.VUTILS) $(LINK.RTP) $(LINK.GWR) $(LINK.ICF) $(LINK.THREADS) $(LINK.ACODECS) $(LINK.VCODECS)

$(OBJDIR)/codec_test: $(OBJDIR)/codecTest.o $(OBJDIR)/paint.o
	$(LD) -o $@ $^ $(LINK.SDK) $(LINK.VUTILS) $(LINK.RTP) $(LINK.GWR) $(LINK.ICF) $(LINK.THREADS) $(LINK.ACODECS) $(LINK.VCODECS)

$(OBJDIR)/performance_test: $(OBJDIR)/performanceTest.o
	$(LD) -o $@ $^ $(LINK.SDK) $(LINK.VUTILS) $(LINK.RTP) $(LINK.ICF) $(LINK.THREADS) $(LINK.ACODECS) $(LINK.VCODECS)

$(OBJDIR)/fragmenter_test: $(OBJDIR)/fragmenterTest.o  $(OBJDIR)/paint.o
	$(LD) -o $@ $^ $(LINK.SDK) $(LINK.VUTILS) $(LINK.RTP) $(LINK.GWR) $(LINK.ICF) $(LINK.THREADS) $(LINK.ACODECS) $(LINK.VCODECS)

$(OBJDIR)/image_loader_test: $(OBJDIR)/imageLoaderTest.o  $(OBJDIR)/paint.o
	$(LD) -o $@ $^ $(LINK.SDK) $(LINK.VUTILS) $(LINK.RTP) $(LINK.GWR) $(LINK.ICF) $(LINK.THREADS) $(LINK.ACODECS) $(LINK.VCODECS)

$(OBJDIR)/image_saver_test: $(OBJDIR)/unix/imageSaverTest.o  $(OBJDIR)/paint.o
	$(LD) -o $@ $^ $(LINK.SDK) $(LINK.VUTILS) $(LINK.RTP) $(LINK.GWR) $(LINK.ICF) $(LINK.THREADS) $(LINK.ACODECS) $(LINK.VCODECS)

clean::
	rm -f \
	$(OBJDIR)/grabberTest.o \
	$(OBJDIR)/formatTest.o \
	$(OBJDIR)/codecTest.o \
	$(OBJDIR)/performanceTest.o \
	$(OBJDIR)/fragmenterTest.o \
	$(OBJDIR)/imageLoaderTest.o

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
