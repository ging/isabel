USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_Titles

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_TITLES_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
                daemon.itcl \
                state.itcl \
                titles.itcl \
                titlesCC.itcl


$(OBJDIR)/isacomp_Titles: $(ISABEL_TITLES_SRC)
	/bin/cat $(ISABEL_TITLES_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_Titles --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_Titles"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_Titles $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








