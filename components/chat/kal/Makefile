USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_Chat

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_CHAT_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
                chatGUI.itcl \
                chatd.tcl \
                chatCC.itcl

$(OBJDIR)/isacomp_Chat: $(ISABEL_CHAT_SRC)
	/bin/cat $(ISABEL_CHAT_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_Chat --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_Chat"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_Chat $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








