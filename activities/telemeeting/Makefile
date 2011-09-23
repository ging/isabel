include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@echo "meeting.act --> $(DISTRIB_DIR)/isabelcore/lib"
	@cp -f meeting.act $(DISTRIB_DIR)/isabelcore/lib
	@echo "act --> $(DISTRIB_DIR)/isabelcore/lib"
	@cp -rf act $(DISTRIB_DIR)/isabelcore/lib/
	@find $(DISTRIB_DIR)/isabelcore/lib/act  -name .svn -prune -exec rm -rf {} \;

cleanall:: clean
	@rm -f  $(DISTRIB_DIR)/isabelcore/lib/meeting.act 
	@rm -rf $(DISTRIB_DIR)/isabelcore/lib/act/meeting


include $(PROJECT_ROOT_DIR)/Makefile.tail
