include $(PROJECT_ROOT_DIR)/Makefile.head

distrib::
	@echo "Distribuyendo jars en $(DISTRIB_DIR)/isabelcore"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/libexec/xmlrpc
	@cp ./mail-1.4.1.jar   $(DISTRIB_DIR)/isabelcore/libexec/.
	@cp ./xmlrpc/*         $(DISTRIB_DIR)/isabelcore/libexec/xmlrpc/.

include $(PROJECT_ROOT_DIR)/Makefile.tail
