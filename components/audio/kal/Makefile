USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_Audio

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_AUDIO_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
                $(TCL_COMMON)/swc/swc.itk \
                $(TCL_COMMON)/swc/optionmenu.itk \
                $(TCL_COMMON)/swc/switchbutton.itk \
                $(COMPONENTS)/audio/kal/config/codec.itcl \
                $(COMPONENTS)/audio/kal/config/tones.itcl \
                $(COMPONENTS)/audio/kal/config/mixers.itcl \
                $(COMPONENTS)/audio/kal/config/config.itcl \
                $(COMPONENTS)/audio/kal/config/site/siteconfig.itcl \
                $(COMPONENTS)/audio/kal/config/site/devices.itcl \
                $(COMPONENTS)/audio/kal/config/site/pbuffer.itcl \
                $(COMPONENTS)/audio/kal/config/site/silence.itcl \
                $(COMPONENTS)/audio/kal/config/site/supecho.itcl \
                $(COMPONENTS)/audio/kal/config/site/agc.itcl \
                $(COMPONENTS)/audio/kal/config/site/bypass.itcl \
                $(COMPONENTS)/audio/kal/config/site/testing/testinput.itcl \
                $(COMPONENTS)/audio/kal/config/site/testing/testoutput.itcl \
                $(COMPONENTS)/audio/kal/audiod.tcl \
                $(COMPONENTS)/audio/kal/vumeterd.tcl \
                $(COMPONENTS)/audio/kal/mixer.itcl \
                $(COMPONENTS)/audio/kal/aud.tcl \
                $(COMPONENTS)/audio/kal/smsState.itcl \
                $(COMPONENTS)/audio/kal/gui/channelui.itk \
                $(COMPONENTS)/audio/kal/gui/mixerui.itk \
                $(COMPONENTS)/audio/kal/gui/audioui.itk \
                $(COMPONENTS)/audio/kal/audioCC.itcl


$(OBJDIR)/isacomp_Audio: $(ISABEL_AUDIO_SRC)
	/bin/cat $(ISABEL_AUDIO_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_Audio --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_Audio"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_Audio $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail










