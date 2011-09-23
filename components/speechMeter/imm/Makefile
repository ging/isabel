USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_mplugin_SpeechMeter

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_MASTER_PLUGIN_SPEECHMETER_SRC:=\
                speechmeter_imm.itcl


$(OBJDIR)/isacomp_mplugin_SpeechMeter: $(ISABEL_MASTER_PLUGIN_SPEECHMETER_SRC)
	/bin/cat $(ISABEL_MASTER_PLUGIN_SPEECHMETER_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_mplugin_SpeechMeter --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins/isacomp_mplugin_SpeechMeter"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins
	@cp -f $(OBJDIR)/isacomp_mplugin_SpeechMeter $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








