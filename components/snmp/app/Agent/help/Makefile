include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=donewithhelp

SRC=    \
	arranque.html \
	monitorizacion.html \
	comandos.html \
	interfaz.html \
	helpIndex.html


OBJ=$(SRC:%=$(OBJDIR)/%)

$(OBJDIR)/donewithhelp: $(SRC)
	cp $(SRC) $(OBJDIR)
	touch $@


include $(PROJECT_ROOT_DIR)/Makefile.tail

