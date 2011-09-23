USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_Monitor

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_WALLPAPER_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
                $(COMPONENTS)/snmp/kal/monitord.itcl \
                $(COMPONENTS)/snmp/kal/monitor.itcl \
                $(COMPONENTS)/snmp/kal/monitorCC.itcl

$(OBJDIR)/isacomp_Monitor: $(ISABEL_WALLPAPER_SRC)
	/bin/cat $(ISABEL_WALLPAPER_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_Monitor --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_Monitor"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_Monitor $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








