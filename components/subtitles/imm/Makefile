USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_mplugin_Subtitles

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_MASTER_PLUGIN_SUBTITLES_SRC:=\
                subtitles_imm.itcl


$(OBJDIR)/isacomp_mplugin_Subtitles: $(ISABEL_MASTER_PLUGIN_SUBTITLES_SRC)
	/bin/cat $(ISABEL_MASTER_PLUGIN_SUBTITLES_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_mplugin_Subtitles --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins/isacomp_mplugin_Subtitles"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins
	@cp -f $(OBJDIR)/isacomp_mplugin_Subtitles $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








