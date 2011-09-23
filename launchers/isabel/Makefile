include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=java
SUBDIRS.POST=

SCRIPTS:=\
	isabel \
	xlim_launcher \
	isabel_launch \
        isabel.common.sh \
	isabel.environ.sh

PASSWDFILES=mediaserver.passwd sipgateway.passwd

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@for i in $(SCRIPTS) ; do \
	  echo "$$i     --> $(DISTRIB_DIR)/isabelcore/bin"	;\
	  cp -rf $$i $(DISTRIB_DIR)/isabelcore/bin		;\
	  chmod a+x $(DISTRIB_DIR)/isabelcore/bin/$$i		;\
	done
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@for i in $(PASSWDFILES) ; do \
	  echo "$$i     --> $(DISTRIB_DIR)/isabelcore/lib"	;\
	  cp -f $$i $(DISTRIB_DIR)/isabelcore/lib		;\
	  chmod a+r $(DISTRIB_DIR)/isabelcore/lib/$$i 		;\
	done

include $(PROJECT_ROOT_DIR)/Makefile.tail
