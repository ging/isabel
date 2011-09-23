include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=


distrib::
	@for i in isrv_*.sh ; do \
	  mkdir -p $(DISTRIB_DIR)/isabelcore/bin ;\
	  echo "$$i     --> $(DISTRIB_DIR)/isabelcore/bin" ;\
	  cp -f $$i $(DISTRIB_DIR)/isabelcore/bin/.    ;\
	  chmod a+xr $(DISTRIB_DIR)/isabelcore/bin/$$i ;\
	done

include $(PROJECT_ROOT_DIR)/Makefile.tail
