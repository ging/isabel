include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=

SUBDIRS.POST=

TARGET=vncviewer

vncviewer:
	@echo "Compiling vncviwer"
	@cd vnc_unixsrc; xmkmf; make World

distrib::
	@echo "Distribuyendo vnc-stuff en $(DISTRIB_DIR)/isabelcore"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@mkdir -p $(DISTRIB_DIR)/isabelcore/libexec
	@cp ./vnc_unixsrc/vncviewer/vncviewer $(DISTRIB_DIR)/isabelcore/bin/vncShmCapture

clean::
	@echo "Compiling vncviwer"
	@cd vnc_unixsrc; make clean

cleanall:: clean
	@rm -f $(DISTRIB_DIR)/isabelcore/bin/vncShmCapture

include $(PROJECT_ROOT_DIR)/Makefile.tail
