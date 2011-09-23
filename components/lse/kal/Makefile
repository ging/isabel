USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_LSE

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_LSE_CONTROL_SRC:=\
                control/control.itcl

ISABEL_LSE_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
                $(ISABEL_LSE_CONTROL_SRC) \
                lseCC.itcl


$(OBJDIR)/isacomp_LSE: $(ISABEL_LSE_SRC)
	/bin/cat $(ISABEL_LSE_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_LSE --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_LSE"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_LSE $(DISTRIB_DIR)/isabelcore/lib/isatcl/.

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








