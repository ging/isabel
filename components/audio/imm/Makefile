USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_mplugin_Audio

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_MASTER_PLUGIN_AUDIO_SRC:=\
                audio_imm.itcl \
                audio_bypass.itcl


$(OBJDIR)/isacomp_mplugin_Audio: $(ISABEL_MASTER_PLUGIN_AUDIO_SRC)
	/bin/cat $(ISABEL_MASTER_PLUGIN_AUDIO_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_mplugin_Audio --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins/isacomp_mplugin_Audio"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins
	@cp -f $(OBJDIR)/isacomp_mplugin_Audio $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








