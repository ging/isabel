include $(PROJECT_ROOT_DIR)/Makefile.head

ifeq ($(TARGET.OS),Win32)
SUBDIRS.PRE=WinXP
SUBDIRS.POST=
endif

ifeq ($(TARGET.OS),Linux)
SUBDIRS.PRE=app app2 avatar imm kal
SUBDIRS.POST=
endif

include $(PROJECT_ROOT_DIR)/Makefile.tail
