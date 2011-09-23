include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@echo "localtest.act --> $(DISTRIB_DIR)/isabelcore/lib"
	@cp -f localtest.act $(DISTRIB_DIR)/isabelcore/lib
	@echo "sessions --> $(DISTRIB_DIR)/isabelcore/lib/sessions"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/sessions
	@cp sessions/localtest.xedl $(DISTRIB_DIR)/isabelcore/lib/sessions/.
	@find $(DISTRIB_DIR)/isabelcore/lib/act  -name .svn -prune -exec rm -rf {} \;

cleanall:: clean
	@rm -f  $(DISTRIB_DIR)/isabelcore/lib/localtest.act 
	@rm -f  $(DISTRIB_DIR)/isabelcore/lib/sessions/localtest.xedl 

include $(PROJECT_ROOT_DIR)/Makefile.tail
