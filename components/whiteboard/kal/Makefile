USE.LIBS= ITCL
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isacomp_Whiteboard

SUBDIRS.PRE=
SUBDIRS.POST=

ISABEL_WHITEBOARD_SRC:=\
             $(TCL_COMMON)/requires.itcl \
             $(KERNEL_CAL)/includes.itcl \
	     $(TCL_COMMON)/swc/swc.itk \
             $(TCL_COMMON)/swc/optionmenu.itk \
             $(COMPONENTS)/scanner/kal/doscan.itcl \
             $(COMPONENTS)/scanner/kal/preview.itcl \
             $(COMPONENTS)/scanner/kal/sdw.itcl \
             wb_cmanager.tcl \
             wb_state.tcl \
             wb_gui.tcl \
             wb_coordination.tcl \
             wb_syncronize.tcl \
             wb_scanner.tcl \
             wb_figure.tcl \
             wb_bitmap.tcl \
             wb_draw.tcl \
             wb_f_gui.tcl \
             wb_fileio.tcl \
             wb_ftp.tcl \
             wb_group.tcl \
             wb_image.tcl \
             wb_line.tcl \
             wb_oval.tcl \
             wb_polygon.tcl \
             wb_rectangle.tcl \
             wb_saco.tcl \
             wb_text.tcl \
             wb_undo.tcl \
             whiteboardd.tcl \
             whiteboard.tcl \
             whiteboardCC.itcl 


$(OBJDIR)/isacomp_Whiteboard: $(ISABEL_WHITEBOARD_SRC)
	/bin/cat $(ISABEL_WHITEBOARD_SRC) | egrep -v "^#[^d].*[^\\\\]$$" > $@ 

distrib::
	@echo "isacomp_Whiteboard --> $(DISTRIB_DIR)/isabelcore/lib/isatcl/isacomp_Whiteboard"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/isatcl
	@cp -f $(OBJDIR)/isacomp_Whiteboard $(DISTRIB_DIR)/isabelcore/lib/isatcl

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail








