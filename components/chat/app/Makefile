USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_chat 

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_CHAT_SRC:=\
             $(TCL_COMMON)/requires.itcl \
             $(TCL_COMMON)/setcolor.tcl \
             $(TCL_COMMON)/balloon.tcl \
             $(TCL_COMMON)/misc.tcl \
             $(TCL_COMMON)/trace_msg.tcl \
             $(TCL_COMMON)/atexit.tcl \
             $(TCL_COMMON)/msgerror.itcl \
             $(TCL_COMMON)/seco/SeCoSRPacket.itcl \
             $(TCL_COMMON)/seco/SeCoQueueSession.itcl \
             $(TCL_COMMON)/seco/SeCoSession.itcl \
             $(TCL_COMMON)/seco/SeCo.itcl \
             $(TCL_COMMON)/pings.tcl \
             $(TCL_COMMON)/hsocket.tcl \
             $(TCL_COMMON)/daemons/dserializer.itcl \
             $(TCL_COMMON)/daemons/daemon.itcl \
             chatGUI.itcl \
             chatd.tcl 

$(OBJDIR)/isabel_chat: $(ISABEL_CHAT_SRC)
	@/bin/cat $(ISABEL_CHAT_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isabel_chat --> $(DISTRIB_DIR)/lib/isatcl/."
	@cp -f $(OBJDIR)/isabel_chat $(DISTRIB_DIR)/lib/isatcl
	@chmod a+x $(DISTRIB_DIR)/lib/isatcl/isabel_chat

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
