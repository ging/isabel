USE.LIBS= ICF2-NON-THREADED ACODECS THREADS X11 STD
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET = vumeter

vumeter_SOURCES = XVumeter.cxx \
		  vumeter.cxx \
		  CtrlInterface.cxx \
		  VumeterClient.cxx \
		  JitterBuffer.cxx \
		  Thread.cxx \
		  Time.cxx \
		  Socket.cxx

vumeter_OBJ = $(vumeter_SOURCES:%.cxx=$(OBJDIR)/%.o)

$(OBJDIR)/vumeter: $(vumeter_OBJ)
	$(CXX) -o $@ $(vumeter_OBJ) \
	$(LINK.Thread) $(LINK.ACODECS) \
	$(LINK.FLTK) $(LINK.X11) $(LINK.THREADS) $(LINK.STD)

clean::
	rm -f $(vumeter_OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

distrib::
	@echo "vumeter --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp -f $(OBJDIR)/vumeter $(DISTRIB_DIR)/isabelcore/bin/isabel_vumeter
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_vumeter
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_vumeter


include $(PROJECT_ROOT_DIR)/Makefile.tail
















