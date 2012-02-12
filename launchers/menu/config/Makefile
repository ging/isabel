include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=


distrib::
	@echo       "menu.config --> $(DISTRIB_DIR)/isabelcore/lib/menu.config"
	@mkdir -p                "$(DISTRIB_DIR)/isabelcore/lib/"
	@cp menu.config          "$(DISTRIB_DIR)/isabelcore/lib/."

cleanall:: clean
	rm -rf $(DISTRIB_DIR)/isabelcore/lib/menu.config


include $(PROJECT_ROOT_DIR)/Makefile.tail

