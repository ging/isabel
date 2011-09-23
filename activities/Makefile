include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=lib localtest telemeeting teleclass teleconference
SUBDIRS.POST=

distrib::
	@echo "act.qualities --> $(DISTRIB_DIR)/isabelcore/lib/act"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/act
	@cp act.qualities $(DISTRIB_DIR)/isabelcore/lib/act/.

cleanall:: clean
	@rm -f $(DISTRIB_DIR)/isabelcore/lib/act/act.qualities


include $(PROJECT_ROOT_DIR)/Makefile.tail
