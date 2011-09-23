include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelgw/libexec
	@echo "../lib/red5.jar --> $(DISTRIB_DIR)/isabelgw/libexec/isabel_red5.jar"
	@cp -f ../lib/red5.jar $(DISTRIB_DIR)/isabelgw/libexec/isabel_red5.jar
	@echo "../lib/xuggle-xuggler.jar --> $(DISTRIB_DIR)/isabelgw/libexec/xuggle-xuggler.jar"
	@cp -f ../lib/xuggle-xuggler.jar $(DISTRIB_DIR)/isabelgw/libexec/xuggle-xuggler.jar
	@echo "../lib/commons-cli.jar --> $(DISTRIB_DIR)/isabelgw/libexec/commons-cli.jar"
	@cp -f ../lib/commons-cli.jar $(DISTRIB_DIR)/isabelgw/libexec/commons-cli.jar


cleanall:: clean
	@rm -f  $(DISTRIB_DIR)/isabelgw/libexec/isabel_red5.jar
	@rm -f  $(DISTRIB_DIR)/isabelgw/libexec/xuggle-xuggler.jar
	@rm -f  $(DISTRIB_DIR)/isabelgw/libexec/commons-cli.jar


include $(PROJECT_ROOT_DIR)/Makefile.tail
