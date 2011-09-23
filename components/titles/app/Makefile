USE.LIBS= ICF2-NON-THREADED
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=titles

SRC=    \
	titlesApp.cc \
	ctrlProtocol.cc \
	txtWin.cc \
	txtWinMgr.cc \
	color.cc \
	main.cc

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


$(OBJDIR)/titles: $(OBJ)
	$(LD) -o $@ $(OBJ) $(LINK.ICF) $(LINK.X11) -lXext -lXmu

distrib::
	@echo "isabel_titles --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp -f $(OBJDIR)/titles $(DISTRIB_DIR)/isabelcore/bin/isabel_titles
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_titles
	@chmod 555 $(DISTRIB_DIR)/isabelcore/bin/isabel_titles

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

