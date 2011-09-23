include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=\
	vnc \
	gethostbyname \
	error \
	sum \
	localaddress \
        vpn \
        updates \
        versionmgr \
        tunnel \
        winunder

SUBDIRS.POST=

distrib::
	@echo "X11/rgb.txt --> $(DISTRIB_DIR)/isabelcore/lib"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@cp X11/rgb.txt  $(DISTRIB_DIR)/isabelcore/lib

include $(PROJECT_ROOT_DIR)/Makefile.tail

