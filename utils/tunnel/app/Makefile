include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_openvpn

SUBDIRS.PRE=
SUBDIRS.POST=


SRC=isabel_openvpn.c 
OBJ=$(SRC:%.c=$(OBJDIR)/%.o)


$(OBJDIR)/isabel_openvpn: $(OBJ)
	$(CXX) -o $@ $(OBJ) 

distrib::
	@echo "isabel_openvpn --> $(DISTRIB_DIR)/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp -f $(OBJDIR)/isabel_openvpn $(DISTRIB_DIR)/isabelcore/bin
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_openvpn
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_openvpn

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
