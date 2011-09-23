CONFIG.generic.include+=-I../pbm -I../pgm -I../ppm -I../

include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=libppm.a isabel_ppmtogif isabel_ppmquant

CXXFLAGS += -x c

PPM_SRC=\
	libppm1.c \
	libppm2.c \
	libppm3.c \
	libppm4.c

PPM_OBJ=$(PPM_SRC:%.c=$(OBJDIR)/%.o)

TOGIF_SRC=ppmtogif.c
TOGIF_OBJ=$(TOGIF_SRC:%.c=$(OBJDIR)/%.o)

QUANT_SRC=ppmquant.c
QUANT_OBJ=$(QUANT_SRC:%.c=$(OBJDIR)/%.o)


$(OBJDIR)/libppm.a: $(PPM_OBJ)
	ar cr $@ $?

LIB_PBM:=$(OBJDIR)/../pbm/
LIB_PGM:=$(OBJDIR)/../pgm/
LIB_PPM:=$(OBJDIR)/../ppm/

EXTRA_LIBS:=\
	$(LIB_PPM)/libppm.a \
	$(LIB_PGM)/libpgm.a \
	$(LIB_PBM)/libpbm.a

$(OBJDIR)/isabel_ppmtogif: $(TOGIF_OBJ)
	$(CXX) -o $@ $(TOGIF_OBJ) $(EXTRA_LIBS) 

$(OBJDIR)/isabel_ppmquant: $(QUANT_OBJ)
	$(CXX) -o $@ $(QUANT_OBJ) $(EXTRA_LIBS)

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@echo "isabel_ppmtogif --> $(DISTRIB_DIR)/isabelcore/bin"
	@cp -f $(OBJDIR)/isabel_ppmtogif $(DISTRIB_DIR)/isabelcore/bin
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_ppmtogif
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_ppmtogif
	@echo "isabel_ppmquant --> $(DISTRIB_DIR)/isabelcore/bin"
	@cp -f $(OBJDIR)/isabel_ppmquant $(DISTRIB_DIR)/isabelcore/bin
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_ppmquant
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_ppmquant

clean::
	rm -f $(TOGIF_OBJ) $(QUANT_OBJ) $(PPM_OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
