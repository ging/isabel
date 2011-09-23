include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

LIB_FILES=options_Default.xml

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/xlimconfig
	@for i in $(LIB_FILES) ; do \
	  echo "$$i     --> $(DISTRIB_DIR)/isabelcore/lib/xlimconfig/." ;\
	  chmod a+r $$i				;\
	  cp -f $$i $(DISTRIB_DIR)/isabelcore/lib/xlimconfig/.		;\
	done

include $(PROJECT_ROOT_DIR)/Makefile.tail
