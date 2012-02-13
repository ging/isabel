USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_master

SUBDIRS.PRE=
SUBDIRS.POST=


ISABEL_MASTER_SRC:=\
                $(TCL_COMMON)/requires.itcl \
                $(TCL_COMMON)/misc.tcl \
                $(TCL_COMMON)/setfont.itcl \
                $(TCL_COMMON)/setcolor.tcl \
                $(TCL_COMMON)/CBManager.itcl \
                $(TCL_COMMON)/msgerror.itcl \
                $(TCL_COMMON)/seco/SeCoSRPacket.itcl \
                $(TCL_COMMON)/seco/SeCoQueueSession.itcl \
                $(TCL_COMMON)/seco/SeCoSession.itcl \
                $(TCL_COMMON)/seco/SeCo.itcl \
                $(TCL_COMMON)/customercfg.itcl \
                $(KERNEL_COMMON)/environ.itcl \
                $(TCL_COMMON)/trace_msg.tcl \
                $(TCL_COMMON)/atexit.tcl \
                $(TCL_COMMON)/balloon.tcl \
                $(KERNEL_MASTER)/services.itcl \
                $(KERNEL_COMMON)/interface.itcl \
                $(KERNEL_MASTER)/interface.itcl \
                $(KERNEL_COMMON)/network.itcl \
                $(KERNEL_COMMON)/globalState.itcl \
                $(KERNEL_MASTER)/compManager.itcl \
                $(KERNEL_MASTER)/siteManager.itcl \
                $(KERNEL_MASTER)/autoquit.itcl \
                $(KERNEL_COMMON)/siteInfo.itcl \
                $(KERNEL_COMMON)/activity.itcl \
                $(KERNEL_MASTER)/activity.itcl \
                $(KERNEL_COMMON)/sessionInfo.itcl \
                $(KERNEL_COMMON)/../sms/SmsStateCounter.itcl \
                $(KERNEL_COMMON)/../sms/SmsState.itcl \
                $(KERNEL_COMMON)/../sms/SmsManagerMaster.itcl \
                $(KERNEL_MASTER)/cmanager.itcl \
                $(TCL_COMMON)/keepalive.itcl \
                $(TCL_COMMON)/dvirtual.itcl \
                \
                $(KERNEL_COMMON)/../sms/SmsManagerClient.itcl \
                \
                $(KERNEL_MASTER)/master.tcl 

$(OBJDIR)/isabel_master: $(ISABEL_MASTER_SRC)
	@/bin/cat $(ISABEL_MASTER_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isabel_master --> $(DISTRIB_DIR)/isabelcore/lib/isatcl"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isabel_master $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@chmod a+x $(DISTRIB_DIR)/isabelcore/lib/isatcl/isabel_master

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
