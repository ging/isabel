USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_update

SUBDIRS.PRE=
SUBDIRS.POST=


ISABEL_UPDATE_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(TCL_COMMON)/misc.tcl \
                $(TCL_COMMON)/setcolor.tcl \
                isabel_update.itcl


$(OBJDIR)/isabel_update: $(ISABEL_UPDATE_SRC)
	@/bin/cat $(ISABEL_UPDATE_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isabel_update --> $(DISTRIB_DIR)/isabelcore/lib/isatcl"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isabel_update $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








