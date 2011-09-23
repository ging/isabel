USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_interactive

SUBDIRS.PRE=
SUBDIRS.POST=


ISABEL_INTERACTIVE_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(TCL_COMMON)/misc.tcl \
                $(TCL_COMMON)/setfont.itcl \
                $(TCL_COMMON)/setcolor.tcl \
                $(TCL_COMMON)/hsocket.tcl \
                $(TCL_COMMON)/CBManager.itcl \
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
                $(KERNEL_INTERACTIVE)/services.itcl \
                $(KERNEL_COMMON)/network.itcl \
                $(KERNEL_COMMON)/globalState.itcl \
                $(KERNEL_COMMON)/interface.itcl \
                $(KERNEL_INTERACTIVE)/interface.itcl \
                $(KERNEL_INTERACTIVE)/compManager.itcl \
                $(KERNEL_INTERACTIVE)/siteManager.itcl \
                $(KERNEL_COMMON)/siteInfo.itcl \
                $(KERNEL_INTERACTIVE)/componentStub.itcl \
                $(KERNEL_COMMON)/sessionInfo.itcl \
                $(KERNEL_COMMON)/activity.itcl \
                $(KERNEL_COMMON)/../sms/SmsStateCounter.itcl \
                $(KERNEL_COMMON)/../sms/SmsManagerClient.itcl \
                $(KERNEL_INTERACTIVE)/admin.itcl \
                $(KERNEL_INTERACTIVE)/views.tcl \
                $(TCL_COMMON)/keepalive.itcl \
                $(COMPONENTS)/irouter/kal/irouterd.tcl \
                $(COMPONENTS)/irouter/kal/trafficLogs/itl.itcl \
                $(TCL_COMMON)/external.itcl \
                $(TCL_COMMON)/dvirtual.itcl \
                $(COMPONENTS)/irouter/kal/iroutertask.itcl \
                $(COMPONENTS)/irouter/kal/config.itcl \
                $(KERNEL_INTERACTIVE)/isabel.tcl 




$(OBJDIR)/isabel_interactive: $(ISABEL_INTERACTIVE_SRC)
	@/bin/cat $(ISABEL_INTERACTIVE_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isabel_interactive --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isabel_interactive"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isabel_interactive $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@chmod a+x $(DISTRIB_DIR)/isabelcore/lib/isatcl/isabel_interactive

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








