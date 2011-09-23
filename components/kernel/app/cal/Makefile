USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_cal

SUBDIRS.PRE=
SUBDIRS.POST=


ISABEL_CAL_SRC:=\
                $(TCL_COMMON)/misc.tcl \
                $(TCL_COMMON)/setfont.itcl \
                $(TCL_COMMON)/setcolor.tcl \
                $(TCL_COMMON)/hsocket.tcl \
                $(TCL_COMMON)/CBManager.itcl \
                $(TCL_COMMON)/parecidos.itcl \
                $(TCL_COMMON)/msgerror.itcl \
                $(TCL_COMMON)/seco/SeCoSRPacket.itcl \
                $(TCL_COMMON)/seco/SeCoQueueSession.itcl \
                $(TCL_COMMON)/seco/SeCoSession.itcl \
                $(TCL_COMMON)/seco/SeCo.itcl \
                $(TCL_COMMON)/daemons/dserializer.itcl \
                $(TCL_COMMON)/daemons/daemon.itcl \
                $(TCL_COMMON)/customercfg.itcl \
                $(KERNEL_COMMON)/environ.itcl \
                $(TCL_COMMON)/trace_msg.tcl \
                $(TCL_COMMON)/atexit.tcl \
                $(TCL_COMMON)/pings.tcl \
                $(TCL_COMMON)/balloon.tcl \
                $(KERNEL_CAL)/Component.itcl \
                $(KERNEL_CAL)/services.itcl \
                $(KERNEL_COMMON)/network.itcl \
                $(KERNEL_CAL)/siteManager.itcl \
                $(KERNEL_COMMON)/siteInfo.itcl \
                $(KERNEL_COMMON)/sessionInfo.itcl \
                $(KERNEL_COMMON)/activity.itcl \
                $(KERNEL_COMMON)/../sms/SmsStateCounter.itcl \
                $(KERNEL_COMMON)/../sms/SmsManagerClient.itcl \
                $(TCL_COMMON)/external.itcl \
                $(TCL_COMMON)/dvirtual.itcl \
                $(KERNEL_CAL)/cal.itcl 



$(OBJDIR)/isabel_cal: $(ISABEL_CAL_SRC)
	@/bin/cat $(ISABEL_CAL_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isabel_cal --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isabel_cal"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isabel_cal $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








