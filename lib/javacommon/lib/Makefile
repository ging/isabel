include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@echo "logging.properties --> $(DISTRIB_DIR)/isabelcore/lib"
	@cp -f logging.properties $(DISTRIB_DIR)/isabelcore/lib/.
	@echo "logback.xml --> $(DISTRIB_DIR)/isabelcore/lib"
	@cp -f logback.xml $(DISTRIB_DIR)/isabelcore/lib/.


cleanall:: clean
	rm -f $(DISTRIB_DIR)/lib/logging.properties
	rm -f $(DISTRIB_DIR)/lib/logback.xml 

include $(PROJECT_ROOT_DIR)/Makefile.tail

