include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelgw/bin
	@echo "httpliveencoding.sh --> $(DISTRIB_DIR)/isabelgw/bin"
	@cp -f httpliveencoding.sh $(DISTRIB_DIR)/isabelgw/bin/
	@echo "xservervnc.sh --> $(DISTRIB_DIR)/isabelgw/bin"
	@cp -f xservervnc.sh $(DISTRIB_DIR)/isabelgw/bin/



cleanall:: clean
	@rm -f  $(DISTRIB_DIR)/isabelgw/bin/httpliveencoding.sh
	@rm -f  $(DISTRIB_DIR)/isabelgw//bin/xservervnc.sh


include $(PROJECT_ROOT_DIR)/Makefile.tail
