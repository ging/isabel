include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_fastpbm

SUBDIRS.PRE=
SUBDIRS.POST=

CXXFLAGS += -x c

SRC=fast-pbm2gif.c
OBJ=$(SRC:%.c=$(OBJDIR)/%.o)

$(OBJDIR)/isabel_fastpbm: $(OBJ)
	$(CXX) -o $@ $(OBJ) 

distrib::
	@echo "isabel_fastpbm --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp -f $(OBJDIR)/isabel_fastpbm $(DISTRIB_DIR)/isabelcore/bin
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_fastpbm
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_fastpbm

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
