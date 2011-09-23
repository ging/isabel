include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE= CellB H263 JPEG
SUBDIRS.POST=

ifeq ($(TARGET.OS),Darwin)
XVID_CONFIGURE_FLAGS= --disable-assembly
endif

ifeq ($(TARGET.OS),SunOS)
XVID_CONFIGURE_FLAGS= --disable-assembly
endif

distrib::

clean::

cleanall:: clean


include $(PROJECT_ROOT_DIR)/Makefile.tail
