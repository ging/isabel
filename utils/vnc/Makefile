include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=vncviewer reflector/isabel_vncreflector passwdgen/isabel_vncpasswdgen

vncviewer:
	@echo "Compiling vncviwer"
	@cd vnc_unixsrc; xmkmf; make World

reflector/isabel_vncreflector:
	@echo "Compiling reflector"
	@cd reflector; make

passwdgen/isabel_vncpasswdgen:
	@echo "Compiling passwdgen"
	@cd passwdgen; make


distrib::
	@echo "Distribuyendo vnc-stuff en $(DISTRIB_DIR)/isabelcore"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@mkdir -p $(DISTRIB_DIR)/isabelcore/libexec
	@cp ./passwdgen/isabel_vncpasswdgen   $(DISTRIB_DIR)/isabelcore/bin/.
	@cp ./reflector/isabel_vncreflector   $(DISTRIB_DIR)/isabelcore/bin/.
	@cp ./vnc_unixsrc/vncviewer/vncviewer $(DISTRIB_DIR)/isabelcore/bin/vncShmCapture

clean::
	@echo "Compiling vncviwer"
	@cd vnc_unixsrc; make clean
	@echo "Compiling reflector"
	@cd reflector; make clean
	@echo "Compiling passwdgen"
	@cd passwdgen; make clean

cleanall:: clean

include $(PROJECT_ROOT_DIR)/Makefile.tail
