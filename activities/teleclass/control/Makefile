USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_ClassCtrl

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_CLASSCTRL_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
                classCtrlCC.itcl \
                modes.itcl \
                cmds.itcl \
                video.itcl \
                from.itcl \
                main.itcl

$(OBJDIR)/isacomp_ClassCtrl: $(ISABEL_CLASSCTRL_SRC)
	@/bin/cat $(ISABEL_CLASSCTRL_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "$(TARGET) --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/$(TARGET)"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/$(TARGET) $(DISTRIB_DIR)/isabelcore/lib/isatcl/.

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
