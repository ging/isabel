include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=donewithimages

SRC=    \
	audioIcon.gif \
	audioIF.gif \
	blueball.gif \
	colorConf.gif \
	colorIcon.gif \
	portConf.gif \
	exitIcon.gif \
	exitGUIIcon.gif \
	graphConf.gif \
	graphIcon.gif \
	graphWin.gif \
	irouterIcon.gif \
	irouterIF.gif \
	linecl.gif \
	menuOptions.gif \
	menuBW.gif \
	monitorIcon.gif \
	miscIcon.gif \
	shdisIcon.gif \
	shdisIF.gif \
	videoIcon.gif \
	videoIF.gif


OBJ=$(SRC:%=$(OBJDIR)/%)

$(OBJDIR)/donewithimages: $(SRC)
	cp $(SRC) $(OBJDIR)
	touch $@


include $(PROJECT_ROOT_DIR)/Makefile.tail

