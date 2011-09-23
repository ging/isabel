USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_SubTitles

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_SUBTITLES_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
                server.itcl \
                subtitles.itcl \
                subtitlesCC.itcl


$(OBJDIR)/isacomp_SubTitles: $(ISABEL_SUBTITLES_SRC)
	/bin/cat $(ISABEL_SUBTITLES_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_SubTitles --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_SubTitles"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_SubTitles $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








