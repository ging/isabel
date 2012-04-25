include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@echo "meeting1280.act --> $(DISTRIB_DIR)/isabelcore/lib"
	@cp -f meeting1280.act $(DISTRIB_DIR)/isabelcore/lib
	@echo "act --> $(DISTRIB_DIR)/isabelcore/lib"
	@cp -rf act $(DISTRIB_DIR)/isabelcore/lib/

cleanall:: clean
	@rm -f  $(DISTRIB_DIR)/isabelcore/lib/meeting1280.act 
	@rm -rf $(DISTRIB_DIR)/isabelcore/lib/act/meeting1280


include $(PROJECT_ROOT_DIR)/Makefile.tail
