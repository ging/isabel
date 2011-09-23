USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_SldFtp

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_SLDFTP_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
                $(TCL_COMMON)/browser/tree.itcl \
                $(TCL_COMMON)/browser/list.itcl \
                $(TCL_COMMON)/browser/explorer.itcl \
                $(TCL_COMMON)/browser/browser.itcl \
                config.itcl \
                sldftp.tcl \
                sldftpCC.itcl


$(OBJDIR)/isacomp_SldFtp: $(ISABEL_SLDFTP_SRC)
	/bin/cat $(ISABEL_SLDFTP_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_SldFtp --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_SldFtp"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_SldFtp $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








