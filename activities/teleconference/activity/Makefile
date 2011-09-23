include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@echo "conference.act --> $(DISTRIB_DIR)/isabelcore/lib"
	@cp -f conference.act $(DISTRIB_DIR)/isabelcore/lib
	@echo "act --> $(DISTRIB_DIR)/isabelcore/lib"
	@cp -rf act $(DISTRIB_DIR)/isabelcore/lib/
	@find $(DISTRIB_DIR)/isabelcore/lib/act -name .svn -prune -exec rm -rf {} \;

cleanall:: clean
	@rm -f  $(DISTRIB_DIR)/isabelcore/lib/conference.act 
	@rm -rf $(DISTRIB_DIR)/isabelcore/lib/act/conference

include $(PROJECT_ROOT_DIR)/Makefile.tail
