USE.LIBS= THREADS X11 STD
CONFIG.generic.include+=-I./ 

include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=

TARGET = AudioRec

AudioRec_SOURCES = main.cxx \
		CtrlInterface.cxx

AudioRec_OBJ=$(AudioRec_SOURCES:%.cxx=$(OBJDIR)/%.o)


$(OBJDIR)/AudioRec: $(AudioRec_OBJ)
	$(CXX) -o $@ $(AudioRec_OBJ) $(LINK.THREADS) $(LINK.STD) $(LINK.X11) \
	$(LINK.Thread) 

clean::
	echo -f $(AudioRec_OBJ) 

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

distrib::
	@echo "AudioRec --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp -f $(OBJDIR)/AudioRec $(DISTRIB_DIR)/isabelcore/bin/isabel_AudioRec
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_AudioRec
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_AudioRec


include $(PROJECT_ROOT_DIR)/Makefile.tail
















