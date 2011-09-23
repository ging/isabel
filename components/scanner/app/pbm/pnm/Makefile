CONFIG.generic.include+=-I../pbm -I../pgm -I../ppm -I../

include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=libpnm.a isabel_pnmcrop isabel_pnmrotate 

CXXFLAGS += -x c

PNM_SRC=\
	libpnm1.c \
	libpnm2.c \
	libpnm3.c \
	libpnm4.c

PNM_OBJ=$(PNM_SRC:%.c=$(OBJDIR)/%.o)

CROP_SRC=pnmcrop.c
CROP_OBJ=$(CROP_SRC:%.c=$(OBJDIR)/%.o)

ROTATE_SRC=pnmrotate.c
ROTATE_OBJ=$(ROTATE_SRC:%.c=$(OBJDIR)/%.o)


$(OBJDIR)/libpnm.a: $(PNM_OBJ)
	ar cr $@ $?

LIB_PNM:=$(OBJDIR)/../pnm/
LIB_PBM:=$(OBJDIR)/../pbm/
LIB_PGM:=$(OBJDIR)/../pgm/
LIB_PPM:=$(OBJDIR)/../ppm/

EXTRA_LIBS:=\
	$(LIB_PNM)/libpnm.a \
	$(LIB_PGM)/libpgm.a \
	$(LIB_PPM)/libppm.a \
	$(LIB_PBM)/libpbm.a

$(OBJDIR)/isabel_pnmcrop: $(CROP_OBJ)
	$(CXX) -o $@ $(CROP_OBJ) $(EXTRA_LIBS)

$(OBJDIR)/isabel_pnmrotate: $(ROTATE_OBJ)
	$(CXX) -o $@ $(ROTATE_OBJ) $(EXTRA_LIBS) -lm

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@echo "isabel_pnmcrop --> $(DISTRIB_DIR)/isabelcore/bin"
	@cp -f $(OBJDIR)/isabel_pnmcrop $(DISTRIB_DIR)/isabelcore/bin
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_pnmcrop
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_pnmcrop
	@echo "isabel_pnmrotate --> $(DISTRIB_DIR)/isabelcore/bin"
	@cp -f $(OBJDIR)/isabel_pnmrotate $(DISTRIB_DIR)/isabelcore/bin
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_pnmrotate
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_pnmrotate

clean::
	rm -f $(PNM_OBJ) $(CROP_OBJ) $(ROTATE_OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
