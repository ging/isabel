USE.LIBS= ICF2-NON-THREADED X11
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=XUtils

SRC=	\
	main.cc \
	XConn.cc \
	XUtilsApp.cc \
	ctrlProtocol.cc

OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


$(OBJDIR)/XUtils: $(OBJ)
	 $(LD) -o $@ $(OBJ) $(LINK.ICF) $(LINK.THREADS) $(LINK.X11)

distrib::
	@echo "isabel_XUtils --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp -f $(OBJDIR)/XUtils $(DISTRIB_DIR)/isabelcore/bin/isabel_XUtils
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_XUtils
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_XUtils

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
