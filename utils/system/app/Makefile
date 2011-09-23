include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_system

SUBDIRS.PRE=
SUBDIRS.POST=

SRC=isabel_system.c 
OBJ=$(SRC:%.c=$(OBJDIR)/%.o)

$(OBJDIR)/isabel_system: $(OBJ)
	$(CXX) -o $@ $(OBJ) 

distrib::
	@echo "isabel_system --> $(DISTRIB_DIR)/bin"
	@cp -f $(OBJDIR)/isabel_system $(DISTRIB_DIR)/bin
	@$(STRIP) $(DISTRIB_DIR)/bin/isabel_system
	@chmod a+x $(DISTRIB_DIR)/bin/isabel_system

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
