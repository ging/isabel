include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

SCRIPTS:=\
	versionmgr.sh

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@for i in $(SCRIPTS) ; do \
	  echo "$$i     --> $(DISTRIB_DIR)/isabelcore/bin"	;\
	  chmod a+x $$i				;\
	  cp -rf $$i $(DISTRIB_DIR)/isabelcore/bin		;\
	done


include $(PROJECT_ROOT_DIR)/Makefile.tail
