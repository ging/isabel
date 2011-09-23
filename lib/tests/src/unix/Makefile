USE.LIBS= SDK VUTILS-THREADED RTP-THREADED ICF2-THREADED GWR THREADS X11 VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

ifeq ($(TARGET.OS),Linux)
#TARGET=	imageSaverTest.o
endif
ifeq ($(TARGET.OS),SunOS)
#TARGET=	imageSaverTest.o
endif

clean::
	rm -f \
	$(OBJDIR)/imageSaverTest.o

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
