USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_VNC

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_VNC_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(KERNEL_CAL)/includes.itcl \
                $(COMPONENTS)/nereda/kal/neredad.itcl \
                $(COMPONENTS)/vnc/kal/vncmain.itcl \
                $(COMPONENTS)/vnc/kal/vncviewer.itcl \
                $(COMPONENTS)/vnc/kal/xvnc.itcl \
                $(COMPONENTS)/vnc/kal/selector/selector.itcl \
                $(COMPONENTS)/vnc/kal/selector/localservers.itcl \
                $(COMPONENTS)/vnc/kal/selector/remoteservers.itcl \
                $(COMPONENTS)/vnc/kal/selector/distnet.itcl \
                $(COMPONENTS)/vnc/kal/selector/lsinfo.itcl \
                $(COMPONENTS)/vnc/kal/selector/rsinfo.itcl \
                $(COMPONENTS)/vnc/kal/hint.itcl \
                $(COMPONENTS)/vnc/kal/xutils/xutilsd.itcl \
                $(COMPONENTS)/vnc/kal/xutils/xutils.itcl \
                $(COMPONENTS)/vnc/kal/shdisplay/shdisplayd.tcl \
                $(COMPONENTS)/vnc/kal/shdisplay/shdisplay.tcl \
                $(COMPONENTS)/vnc/kal/shdisplay/config.itcl \
                $(COMPONENTS)/vnc/kal/vncCC.itcl


$(OBJDIR)/isacomp_VNC: $(ISABEL_VNC_SRC)
	/bin/cat $(ISABEL_VNC_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_VNC --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_VNC"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_VNC $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








