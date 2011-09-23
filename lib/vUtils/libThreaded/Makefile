USE.LIBS= RTP-THREADED ICF2-THREADED VUTILS-THREADED VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET= libthreadedicfvutils.so

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

$(OBJDIR)/libthreadedicfvutils.so: $(OBJ)
	$(LD.SHARED) -o $@.$(VUTILS.Version) $(OBJ) -Xlinker -soname=$$(basename $@).$(VUTILS.Version)
	-rm -f $@
	ln -s ./libthreadedicfvutils.so.$(VUTILS.Version) $@

distrib::
	@echo "libthreadedicfvutils.so -> $(DISTRIB_DIR)/isabelcore/lib"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@cp -f $(OBJDIR)/libthreadedicfvutils.so.* $(DISTRIB_DIR)/isabelcore/lib
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/lib/libthreadedicfvutils.so.*
	@chmod 644 $(DISTRIB_DIR)/isabelcore/lib/libthreadedicfvutils.so.*
	@ln -sf libthreadedicfvutils.so.$(VUTILS.Version) $(DISTRIB_DIR)/isabelcore/lib/libthreadedicfvutils.so

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)
	rm -f $(OBJDIR)/libthreadedicfvutils.so*

include $(PROJECT_ROOT_DIR)/Makefile.tail
