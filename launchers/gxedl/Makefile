include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

SCRIPTS:=\
	gxedl.sh \
	localmaster.sh 


distrib::
	@for i in $(SCRIPTS)			;\
	do					\
	  echo "$$i     --> $(DISTRIB_DIR)/bin"	;\
	  chmod a+x $$i				;\
	  cp -rf $$i $(DISTRIB_DIR)/bin		;\
	done

include $(PROJECT_ROOT_DIR)/Makefile.tail
