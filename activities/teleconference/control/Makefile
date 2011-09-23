USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_ConferenceCtrl

SUBDIRS.PRE=
SUBDIRS.POST=

CONF_ISATCL_SRC:=\
        $(TCL_COMMON)/requires.itcl \
        $(KERNEL_CAL)/includes.itcl \
        $(TCL_COMMON)/browser/tree.itcl \
        $(TCL_COMMON)/browser/list.itcl \
        $(TCL_COMMON)/browser/explorer.itcl \
        $(TCL_COMMON)/browser/browser.itcl \
        conferenceCtrlCC.itcl \
        conference.itcl \
        tc_menubar.itcl	\
        cfapply.itcl \
        connections.itcl

MODES_ISATCL_SRC:=\
        modes/tc_modes.itcl \
        modes/tc_mode.itcl \
        modes/cfmodecmds.itcl 

DETAILS_ISATCL_SRC:=\
        details/details.itcl \
        details/cfusers.tcl \
        details/cfroottitles.itcl \
        details/cfaudio.itcl \
        details/cfspeechmeter.itcl \
        details/cfvnc.itcl \
        details/wallpaper.itcl \
        details/videosite.itcl \
        details/scene.itcl


AGENDA_ISATCL_SRC:=\
        agenda/agenda.itcl \
        agenda/selection.itcl \
        agenda/nodes/lib.itcl \
        agenda/nodes/node.itcl \
        agenda/nodes/conference.itcl \
        agenda/nodes/sessionlist.itcl \
        agenda/nodes/session.itcl \
        agenda/nodes/chair.itcl \
        agenda/nodes/presentation.itcl \
        agenda/nodes/presentationlist.itcl \
        agenda/nodes/person.itcl \
        agenda/nodes/author.itcl \
        agenda/nodes/authorlist.itcl \
        agenda/nodes/personlist.itcl \
        agenda/nodes/resourcelist.itcl \
        agenda/nodes/resource.itcl \
        agenda/nodes/appsharingresource.itcl \
        agenda/nodes/videoresource.itcl \
        agenda/nodes/wallpaperresource.itcl \
        agenda/nodes/scriptitem.itcl \
        agenda/nodes/scriptitemlist.itcl \
        agenda/nodes/site.itcl \
        agenda/nodes/sitelist.itcl \
        agenda/nodes/server.itcl \
        agenda/nodes/serverhint.itcl \
        agenda/carrousel/carrousel.itcl \
        agenda/dialog/frames.itcl 

$(OBJDIR)/isacomp_ConferenceCtrl: $(CONF_ISATCL_SRC) $(DETAILS_ISATCL_SRC) $(AGENDA_ISATCL_SRC) $(MODES_ISATCL_SRC)
	/bin/cat $(CONF_ISATCL_SRC) $(MODES_ISATCL_SRC) $(DETAILS_ISATCL_SRC) $(AGENDA_ISATCL_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_ConferenceCtrl --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_ConferenceCtrl"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_ConferenceCtrl $(DISTRIB_DIR)/isabelcore/lib/isatcl/.

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
