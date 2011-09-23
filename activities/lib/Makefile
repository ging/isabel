include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/act/lib
	@echo "lib/act/lib/* --> $(DISTRIB_DIR)/isabelcore/lib/act/lib/."
	@cp -rf comps        $(DISTRIB_DIR)/isabelcore/lib/act/lib/.
	@cp -rf menuitems    $(DISTRIB_DIR)/isabelcore/lib/act/lib/.
	@cp -rf roles        $(DISTRIB_DIR)/isabelcore/lib/act/lib/.
	@cp -rf vars         $(DISTRIB_DIR)/isabelcore/lib/act/lib/.
	@find $(DISTRIB_DIR)/isabelcore/lib/act/lib -name .svn -prune -exec rm -rf {} \;

cleanall:: clean
	@rm -rf $(DISTRIB_DIR)/isabelcore/lib/act/lib/comps
	@rm -rf $(DISTRIB_DIR)/isabelcore/lib/act/lib/menuitems
	@rm -rf $(DISTRIB_DIR)/isabelcore/lib/act/lib/roles
	@rm -rf $(DISTRIB_DIR)/isabelcore/lib/act/lib/vars

include $(PROJECT_ROOT_DIR)/Makefile.tail
