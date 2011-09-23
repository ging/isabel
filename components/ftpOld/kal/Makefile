USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_Ftp

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_FTP_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
                ftpd.tcl \
                ftp.tcl \
                ftpCC.itcl



$(OBJDIR)/isacomp_Ftp: $(ISABEL_FTP_SRC)
	/bin/cat $(ISABEL_FTP_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_Ftp --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_Ftp"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_Ftp $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
