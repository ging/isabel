USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_error 

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_ERROR_SRC:=\
             $(TCL_COMMON)/requires.itcl \
             $(TCL_COMMON)/setcolor.tcl \
             $(TCL_COMMON)/misc.tcl \
             $(TCL_COMMON)/msgerror.itcl \
             main.itcl 

$(OBJDIR)/isabel_error: $(ISABEL_ERROR_SRC)
	@/bin/cat $(ISABEL_ERROR_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@echo "isabel_error --> $(DISTRIB_DIR)/isabelcore/lib"
	@cp -f $(OBJDIR)/isabel_error $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@chmod a+x $(DISTRIB_DIR)/isabelcore/lib/isatcl/isabel_error

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
