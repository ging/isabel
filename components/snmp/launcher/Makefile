include $(PROJECT_ROOT_DIR)/Makefile.head


SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@echo "isabel_SNMPAgent --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@cp -f ./isabel_SNMPAgent $(DISTRIB_DIR)/isabelcore/bin/
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_SNMPAgent


include $(PROJECT_ROOT_DIR)/Makefile.tail
