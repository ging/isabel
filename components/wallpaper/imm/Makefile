USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_mplugin_Wallpaper

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_MASTER_PLUGIN_WALLPAPER_SRC:=\
                wallpaper_imm.itcl


$(OBJDIR)/isacomp_mplugin_Wallpaper: $(ISABEL_MASTER_PLUGIN_WALLPAPER_SRC)
	/bin/cat $(ISABEL_MASTER_PLUGIN_WALLPAPER_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_mplugin_Wallpaper --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins/isacomp_mplugin_Wallpaper"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins
	@cp -f $(OBJDIR)/isacomp_mplugin_Wallpaper $(DISTRIB_DIR)/isabelcore/lib/isatcl/masterplugins

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








