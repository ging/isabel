include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@for i in gif xbm jpg png xlim xedlviewer ; do \
	   echo "$$i     --> $(DISTRIB_DIR)/isabelcore/lib/images/$$i" ;\
	   mkdir -p $(DISTRIB_DIR)/isabelcore/lib/images/$$i           ;\
	   cp -rf $$i $(DISTRIB_DIR)/isabelcore/lib/images             ;\
	   chmod 755 $(DISTRIB_DIR)/isabelcore/lib/images/$$i/*        ;\
	done
	@find $(DISTRIB_DIR)/isabelcore/lib/images -name .svn -prune -exec rm -rf {} \;

cleanall::
	@for i in camcontrol gif xbm jpg png xlim xedlviewer ; do \
	   rm -rf $(DISTRIB_DIR)/isabelcore/lib/images/$$i             ;\
	done

include $(PROJECT_ROOT_DIR)/Makefile.tail
