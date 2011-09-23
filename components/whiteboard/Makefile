include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=kal
SUBDIRS.POST=

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/images/whiteboard/
	@chmod 755 $(DISTRIB_DIR)/isabelcore/lib/images/whiteboard
	@cp -rf images/*  $(DISTRIB_DIR)/isabelcore/lib/images/whiteboard/
	@chmod 755 $(DISTRIB_DIR)/isabelcore/lib/images/whiteboard/*

include $(PROJECT_ROOT_DIR)/Makefile.tail
