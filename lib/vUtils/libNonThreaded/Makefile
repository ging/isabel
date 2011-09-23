USE.LIBS= RTP-NON-THREADED ICF2-NON-THREADED VUTILS-NON-THREADED VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET= libnonthreadedicfvutils.so

FILES=  \
        image.cc            \
        rtp2image.cc        \
        imgTranslator.cc    \
        coderWrapper.cc

SRC=$(FILES:%.cc=../src/%.cc)
OBJ=$(FILES:%.cc=$(OBJDIR)/%.o)

$(OBJDIR)/%.o:  ../src/%.cc
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

VUTILS.Version:=$(shell cat ../version)

$(OBJDIR)/libnonthreadedicfvutils.so: $(OBJ)
	$(LD.SHARED) -o $@.$(VUTILS.Version) $(OBJ) -Xlinker -soname=$$(basename $@).$(VUTILS.Version)
	-rm -f $@
	ln -s ./libnonthreadedicfvutils.so.$(VUTILS.Version) $@

distrib::
	@echo "libnonthreadedicfvutils.so -> $(DISTRIB_DIR)/isabelcore/lib"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@cp -f $(OBJDIR)/libnonthreadedicfvutils.so.* $(DISTRIB_DIR)/isabelcore/lib
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/lib/libnonthreadedicfvutils.so.*
	@chmod 644 $(DISTRIB_DIR)/isabelcore/lib/libnonthreadedicfvutils.so.*
	@ln -sf libnonthreadedicfvutils.so.$(VUTILS.Version) $(DISTRIB_DIR)/isabelcore/lib/libnonthreadedicfvutils.so

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)
	rm -f $(OBJDIR)/libnonthreadedicfvutils.so*

include $(PROJECT_ROOT_DIR)/Makefile.tail
