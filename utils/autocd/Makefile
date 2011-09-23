include $(PROJECT_ROOT_DIR)/Makefile.head


LINUX_VER := $(findstring Ubuntu, $(shell cat /etc/lsb-release))


ifeq ($(LINUX_VER),Ubuntu)
   
   $(info Selecting Ubuntu compilation)
   SUBDIRS.PRE=ubuntu
   SUBDIRS.POST=

else
   SUBDIRS.PRE=
   SUBDIRS.POST=
endif


include $(PROJECT_ROOT_DIR)/Makefile.tail

