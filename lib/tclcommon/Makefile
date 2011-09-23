include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=


FILES2DISTRIB=tkXdefaultsBlue \
              tkXdefaultsBlue2 \
              tkXdefaultsYellow


distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@for i in $(FILES2DISTRIB) ; do \
	  echo "$$i     --> $(DISTRIB_DIR)/isabelcore/lib"	;\
	  cp $$i $(DISTRIB_DIR)/isabelcore/lib/.  ;\
	done
	@ln -sf  tkXdefaultsBlue $(DISTRIB_DIR)/isabelcore/lib/tkXdefaults

include $(PROJECT_ROOT_DIR)/Makefile.tail
