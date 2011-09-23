USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_Video

SUBDIRS.PRE=
SUBDIRS.POST=


ISABEL_VIDEO_DAEMON_SRC:=\
                $(COMPONENTS)/video/kal/daemon/videod.itcl \
                $(COMPONENTS)/video/kal/daemon/videodaemontask.itcl

ISABEL_VIDEO_CAMERA_SRC:=\
                $(COMPONENTS)/video/kal/camera/config.itcl \
                $(COMPONENTS)/video/kal/camera/source.itcl \
                $(COMPONENTS)/video/kal/camera/info.itcl \
                $(COMPONENTS)/video/kal/camera/vd.itcl \
                $(COMPONENTS)/video/kal/camera/vc.itcl \
                $(COMPONENTS)/video/kal/camera/vg.itcl \
                $(COMPONENTS)/video/kal/camera/camera.itcl

ISABEL_VIDEO_VGA_SRC:=\
                $(COMPONENTS)/video/kal/vga/config.itcl \
                $(COMPONENTS)/video/kal/vga/source.itcl \
                $(COMPONENTS)/video/kal/vga/info.itcl \
                $(COMPONENTS)/video/kal/vga/vd.itcl \
                $(COMPONENTS)/video/kal/vga/vc.itcl \
                $(COMPONENTS)/video/kal/vga/vg.itcl \
                $(COMPONENTS)/video/kal/vga/vga.itcl

ISABEL_VIDEO_LSE_SRC:=\
                $(COMPONENTS)/video/kal/lse/config.itcl \
                $(COMPONENTS)/video/kal/lse/info.itcl \
                $(COMPONENTS)/video/kal/lse/vd.itcl \
                $(COMPONENTS)/video/kal/lse/vc.itcl \
                $(COMPONENTS)/video/kal/lse/vg.itcl \
                $(COMPONENTS)/video/kal/lse/lse.itcl

ISABEL_VIDEO_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
                $(ISABEL_VIDEO_DAEMON_SRC) \
                $(ISABEL_VIDEO_CAMERA_SRC) \
                $(ISABEL_VIDEO_VGA_SRC) \
                $(ISABEL_VIDEO_LSE_SRC) \
                $(COMPONENTS)/video/kal/vconf.itcl \
                $(COMPONENTS)/video/kal/info.itcl \
                $(COMPONENTS)/video/kal/source.itcl \
                $(COMPONENTS)/video/kal/videoCC.itcl

$(OBJDIR)/isacomp_Video: $(ISABEL_VIDEO_SRC)
	/bin/cat $(ISABEL_VIDEO_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_Video --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_Video"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_Video $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








