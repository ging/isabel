USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_gpl

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_GPL_SRC:=\
             $(TCL_COMMON)/requires.itcl \
             $(TCL_COMMON)/setcolor.tcl \
             $(TCL_COMMON)/misc.tcl \
	     gpl_license.itcl

$(OBJDIR)/isabel_gpl: $(ISABEL_GPL_SRC)
	@/bin/cat $(ISABEL_GPL_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@echo "isabel_gpl --> $(DISTRIB_DIR)/isabelcore/lib/isatcl"
	@cp -f $(OBJDIR)/isabel_gpl $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@chmod a+x $(DISTRIB_DIR)/isabelcore/lib/isatcl/isabel_gpl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
