USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_mplugin_Questions

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_MASTER_PLUGIN_QUESTIONS_SRC:=\
                questions_imm.itcl


$(OBJDIR)/isacomp_mplugin_Questions: $(ISABEL_MASTER_PLUGIN_QUESTIONS_SRC)
	/bin/cat $(ISABEL_MASTER_PLUGIN_QUESTIONS_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_mplugin_Questions --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins/isacomp_mplugin_Questions"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins
	@cp -f $(OBJDIR)/isacomp_mplugin_Questions $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








