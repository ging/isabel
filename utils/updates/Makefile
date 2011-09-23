include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=app
SUBDIRS.POST=

distrib::
	@echo "isabel_sourceslist.sh --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@chmod +x isabel_sourceslist.sh
	@cp -f isabel_sourceslist.sh $(DISTRIB_DIR)/isabelcore/bin/.

include $(PROJECT_ROOT_DIR)/Makefile.tail
