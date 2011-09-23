include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=libThreaded libNonThreaded
SUBDIRS.POST=

tgz::
	@echo "packing ICF2 --> $(DISTRIB_DIR)/paq/ICF2.tgz"
	@mkdir -p $(DISTRIB_DIR)/paq
	cd ..; tar cvfz $(DISTRIB_DIR)/paq/ICF2.tgz \
	ICF2/include/icf2/*h \
	ICF2/doc/ICF2.txt \
        --directory $(OBJDIR)/.. ICF2/libNonThreaded/libnonthreadedicf.a \
        ICF2/libThreaded/libthreadedicf.a 

include $(PROJECT_ROOT_DIR)/Makefile.tail
