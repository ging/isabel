include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_sum

SUBDIRS.PRE=
SUBDIRS.POST=

CXXFLAGS += -x c


SRC=sum.c
OBJ=$(SRC:%.c=$(OBJDIR)/%.o)

$(OBJDIR)/isabel_sum: $(OBJ)
	$(CXX) -o $@ $(OBJ) 

distrib::
	@echo "isabel_sum --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp -f $(OBJDIR)/isabel_sum $(DISTRIB_DIR)/isabelcore/bin
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_sum
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_sum

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
