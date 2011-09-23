USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_mplugin_Titles

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_MASTER_PLUGIN_TITLES_SRC:=\
                titles_imm.itcl


$(OBJDIR)/isacomp_mplugin_Titles: $(ISABEL_MASTER_PLUGIN_TITLES_SRC)
	/bin/cat $(ISABEL_MASTER_PLUGIN_TITLES_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_mplugin_Titles --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins/isacomp_mplugin_Titles"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins
	@cp -f $(OBJDIR)/isacomp_mplugin_Titles $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








