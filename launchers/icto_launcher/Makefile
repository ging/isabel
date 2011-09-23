include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

SCRIPTS:=\
	icto_launcher 

FIREFOX_CONF=isabel.js

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@for i in $(SCRIPTS) ; do \
	  echo "$$i     --> $(DISTRIB_DIR)/isabelcore/bin"	;\
	  cp -rf $$i $(DISTRIB_DIR)/isabelcore/bin		;\
	  chmod a+x $(DISTRIB_DIR)/isabelcore/bin/$$i		;\
	done
	@mkdir -p "$(DISTRIB_DIR)/isabelcore/env/etc/firefox/pref"
	@for i in $(FIREFOX_CONF) ; do \
	  echo "$$i     --> $(DISTRIB_DIR)/isabelcore/env//etc/firefox/pref"	;\
	  cp -f $$i $(DISTRIB_DIR)/isabelcore/env/etc/firefox/pref/.		;\
	done


cleanall:: clean
	@for i in $(SCRIPTS) ; do \
	  echo "deleting $(DISTRIB_DIR)/isabelcore/bin/$$i" ;\
	  rm -f $(DISTRIB_DIR)/isabelcore/bin/$$i		;\
	done
	@for i in $(FIREFOX_CONF) ; do \
	  echo "deleting $(DISTRIB_DIR)/isabelcore/env/etc/firefox/pref/$$i"	;\
	  rm -f $(DISTRIB_DIR)/isabelcore/env/etc/firefox/pref/$$i		;\
	done


include $(PROJECT_ROOT_DIR)/Makefile.tail
