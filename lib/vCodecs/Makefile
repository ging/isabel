include $(PROJECT_ROOT_DIR)/Makefile.head

ifeq ($(TARGET.OS),Win32)
SUBDIRS.PRE=
endif

ifeq ($(TARGET.OS),Darwin)
SUBDIRS.PRE=src
endif

ifeq ($(TARGET.OS),Linux)
SUBDIRS.PRE=src
endif

ifeq ($(TARGET.OS),SunOS)
SUBDIRS.PRE=src
endif

SUBDIRS.POST=

include $(PROJECT_ROOT_DIR)/Makefile.tail
