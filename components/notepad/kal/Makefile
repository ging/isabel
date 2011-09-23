USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_Notepad

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_NOTEPAD_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
	        $(TCL_COMMON)/swc/swc.itk \
                $(TCL_COMMON)/swc/optionmenu.itk \
                cmanager.tcl \
                gui.tcl \
                cmds.tcl \
                notepadd.tcl \
                notepadCC.itcl


$(OBJDIR)/isacomp_Notepad: $(ISABEL_NOTEPAD_SRC)
	/bin/cat $(ISABEL_NOTEPAD_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_Notepad --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_Notepad"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_Notepad $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








