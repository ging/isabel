include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_bwrotate

SUBDIRS.PRE=
SUBDIRS.POST=

CXXFLAGS += -x c

SRC=main.c
OBJ=$(SRC:%.c=$(OBJDIR)/%.o)

$(OBJDIR)/isabel_bwrotate: $(OBJ)
	$(CXX) -o $@ $(OBJ) 

distrib::
	@echo "isabel_bwrotate --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp -f $(OBJDIR)/isabel_bwrotate $(DISTRIB_DIR)/isabelcore/bin
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_bwrotate
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_bwrotate

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
