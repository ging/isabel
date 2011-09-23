USE.LIBS= SDK VUTILS-THREADED RTP-THREADED ICF2-THREADED THREADS ACODECS VCODECS GWR
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=wallpaper

SRC=    main.cc

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


$(OBJDIR)/wallpaper: $(OBJ)
	$(LD) -o $@ $(OBJ) $(LINK.SDK) $(LINK.GWR) $(LINK.VUTILS) $(LINK.RTP) $(LINK.ICF) $(LINK.ACODECS) $(LINK.VCODECS) $(LINK.X11) -lXext -lXmu $(LINK.THREADS)

distrib::
	@echo "isabel_wallpaper --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp -f $(OBJDIR)/wallpaper $(DISTRIB_DIR)/isabelcore/bin/isabel_wallpaper
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_wallpaper
	@chmod 555 $(DISTRIB_DIR)/isabelcore/bin/isabel_wallpaper

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

