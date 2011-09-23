USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_Questions

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_QUESTIONS_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
                qwidget.itcl \
                questionCC.itcl \
                question.tcl


$(OBJDIR)/isacomp_Questions: $(ISABEL_QUESTIONS_SRC)
	/bin/cat $(ISABEL_QUESTIONS_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_Questions --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_Questions"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_Questions $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








