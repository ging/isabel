USE.LIBS= ICF2-THREADED GWR THREADS X11
include $(PROJECT_ROOT_DIR)/Makefile.head

ifeq ($(TARGET.OS),Linux)
SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=	\
	drawline_test
endif
ifeq ($(TARGET.OS),SunOS)
SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=	\
	drawline_test
endif

$(OBJDIR)/drawline_test: $(OBJDIR)/drawlineTest.o
	$(LD) -o $@ $^ $(LINK.GWR) $(LINK.ICF) $(LINK.THREADS)

clean::
	rm -f \
	$(OBJDIR)/drawlineTest.o

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
