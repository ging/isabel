include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@echo "gwsipi.1.0.0.xml --> $(DISTRIB_DIR)/isabelgw/lib/defaults/."
	@mkdir -p $(DISTRIB_DIR)/isabelgw/lib/defaults
	@cp gwsip.1.0.0.xml $(DISTRIB_DIR)/isabelgw/lib/defaults/.

cleanall:: clean
	@rm -f  $(DISTRIB_DIR)/isabelgw/lib/defaults/gwsip.1.0.0.xml


include $(PROJECT_ROOT_DIR)/Makefile.tail
