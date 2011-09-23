include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@echo "class.act --> $(DISTRIB_DIR)/isabelcore/lib"
	@cp -f class.act $(DISTRIB_DIR)/isabelcore/lib
	@echo "act --> $(DISTRIB_DIR)/isabelcore/lib"
	@cp -rf act $(DISTRIB_DIR)/isabelcore/lib/
	@find $(DISTRIB_DIR)/isabelcore/lib/act  -name .svn -prune -exec rm -rf {} \;

include $(PROJECT_ROOT_DIR)/Makefile.tail
