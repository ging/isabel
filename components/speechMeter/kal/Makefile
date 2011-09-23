USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_SpeechMeter

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_SPEECHMETER_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
                speechmeter.itcl \
                speechmeterCC.itcl


$(OBJDIR)/isacomp_SpeechMeter: $(ISABEL_SPEECHMETER_SRC)
	/bin/cat $(ISABEL_SPEECHMETER_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_SpeechMeter --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_SpeechMeter"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_SpeechMeter $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








