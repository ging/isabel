USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_MCU

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_MCU_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
                $(COMPONENTS)/MCU/kal/mcud.itcl \
                $(COMPONENTS)/MCU/kal/mcudaemontask.itcl \
                $(COMPONENTS)/MCU/kal/ffmpeg/ffmpeg.itcl \
                $(COMPONENTS)/MCU/kal/ffmpeg/streaming.itcl \
                \
                $(COMPONENTS)/MCU/kal/ios/iosclientinfo.itcl \
                $(COMPONENTS)/MCU/kal/ios/desktop_grabber/videodesktopgrabd.itcl \
                $(COMPONENTS)/MCU/kal/ios/desktop_grabber/videodesktopgrabtask.itcl \
                \
                $(COMPONENTS)/MCU/kal/mcu.itcl \
                $(COMPONENTS)/MCU/kal/mcuCC.itcl


$(OBJDIR)/isacomp_MCU: $(ISABEL_MCU_SRC)
	/bin/cat $(ISABEL_MCU_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_MCU --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_MCU"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_MCU $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail










