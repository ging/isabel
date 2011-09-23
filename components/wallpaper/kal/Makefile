USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_Wallpaper

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_WALLPAPER_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
                $(COMPONENTS)/wallpaper/kal/wallpaper.itcl \
                $(COMPONENTS)/wallpaper/kal/wallpaperCC.itcl


$(OBJDIR)/isacomp_Wallpaper: $(ISABEL_WALLPAPER_SRC)
	/bin/cat $(ISABEL_WALLPAPER_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_Wallpaper --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_Wallpaper"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_Wallpaper $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








