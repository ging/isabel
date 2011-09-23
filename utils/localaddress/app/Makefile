include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_localaddress

SUBDIRS.PRE=
SUBDIRS.POST=

SRC=localaddress.c
OBJ=$(SRC:%.c=$(OBJDIR)/%.o)

$(OBJDIR)/isabel_localaddress: $(OBJ)
	$(LD) -o $@ $(OBJ) 

distrib::
	@echo "isabel_localaddress --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin/
	@cp -f $(OBJDIR)/isabel_localaddress $(DISTRIB_DIR)/isabelcore/bin/
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_localaddress
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_localaddress

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
