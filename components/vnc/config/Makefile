include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@echo "vncservers.cfg  --> $(DISTRIB_DIR)/isabelcore/lib/."
	@cp vncservers.cfg $(DISTRIB_DIR)/isabelcore/lib/.
	@echo "localvnc.rc  --> $(DISTRIB_DIR)/isabelcore/lib/."
	@chmod a+x localvnc.rc
	@cp localvnc.rc $(DISTRIB_DIR)/isabelcore/lib/.
	@echo "localvnc.twmrc  --> $(DISTRIB_DIR)/isabelcore/lib/."
	@cp localvnc.twmrc $(DISTRIB_DIR)/isabelcore/lib/.
	@echo "jwmrc  --> $(DISTRIB_DIR)/isabelcore/lib/."
	@cp jwmrc $(DISTRIB_DIR)/isabelcore/lib/.

include $(PROJECT_ROOT_DIR)/Makefile.tail
