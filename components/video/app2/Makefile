USE.LIBS= ACODECS VCODECS SDK VUTILS-THREADED RTP-THREADED ICF2-THREADED THREADS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=shmSrc

SRC=	                  \
	confFile.cc       \
	shmSink.cc        \
	sourceTask.cc     \
	srcMgr.cc         \
	ctrlProtocol.cc   \
	shmSrcApp.cc      \
	main.cc


OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


$(OBJDIR)/shmSrc: $(OBJ)
	$(LD) -o $@ $(OBJ) $(LINK.SDK) $(LINK.VUTILS) $(LINK.RTP) $(LINK.ICF) $(LINK.ACODECS) $(LINK.VCODECS) $(LINK.THREADS)

distrib::
	@echo "isabel_shmSrc --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp -f $(OBJDIR)/shmSrc $(DISTRIB_DIR)/isabelcore/bin/isabel_shmSrc
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_shmSrc
	@chmod 555 $(DISTRIB_DIR)/isabelcore/bin/isabel_shmSrc

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
