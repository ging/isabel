include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@echo "gapi.1.0.0.xml --> $(DISTRIB_DIR)/isabelgw/lib/defaults/."
	@mkdir -p $(DISTRIB_DIR)/isabelgw/lib/defaults
	@cp gapi.1.0.0.xml $(DISTRIB_DIR)/isabelgw/lib/defaults/.

cleanall:: clean
	@rm -f  $(DISTRIB_DIR)/isabelgw/lib/defaults/gapi.1.0.0.xml


include $(PROJECT_ROOT_DIR)/Makefile.tail
