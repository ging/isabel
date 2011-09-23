USE.LIBS= ICF2-NON-THREADED X11
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=winunder

SRC=    main.cc

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


$(OBJDIR)/winunder: $(OBJ)
	$(LD) -o $@ $(OBJ) $(LINK.ICF) $(LINK.X11)

distrib::
	@echo "isabel_winunder --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp -f $(OBJDIR)/winunder $(DISTRIB_DIR)/isabelcore/bin/isabel_winunder
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_winunder
	@chmod 555 $(DISTRIB_DIR)/isabelcore/bin/isabel_winunder

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

