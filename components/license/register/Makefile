USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_register

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_REGISTER_SRC:=\
             $(TCL_COMMON)/requires.itcl \
             $(TCL_COMMON)/setcolor.tcl \
             $(TCL_COMMON)/misc.tcl \
             $(TCL_COMMON)/msgerror.itcl \
             $(TCL_COMMON)/trace_msg.tcl \
             $(TCL_COMMON)/external.itcl \
	     ctes.itcl \
             config.itcl \
             window.itcl \
             fagree.itcl \
             register.itcl 

$(OBJDIR)/isabel_register: $(ISABEL_REGISTER_SRC)
	@/bin/cat $(ISABEL_REGISTER_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@echo "isabel_register --> $(DISTRIB_DIR)/isabelcore/lib/isatcl"
	@cp -f $(OBJDIR)/isabel_register $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@chmod a+x $(DISTRIB_DIR)/isabelcore/lib/isatcl/isabel_register
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/defaults
	@cp -f reglicfg.txt $(DISTRIB_DIR)/isabelcore/lib/defaults/.

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
