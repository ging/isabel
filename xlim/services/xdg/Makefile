include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=


DESKTOP_FILES=\
   IsabelAbout.desktop \
   IsabelGpl.desktop \
   IsabelCleanup.desktop \
   IsabelConnectTo.desktop \
   IsabelContents.desktop \
   IsabelEditLocalConfiguration.desktop\
   IsabelHomePage.desktop \
   IsabelLocalTest.desktop \
   IsabelLogs.desktop \
   IsabelSelectVersion.desktop \
   IsabelStartSessionServer.desktop \
   IsabelUpdate.desktop


OLD_DESKTOP_FILES=\
   IsabelViewCurrentSession.desktop \
   IsabelStoredSessions.desktop \
   IsabelNetworkTest.desktop \
   IsabelLinksManager.desktop

MENU_FILES=*.menu

DIRECTORY_FILES=*.directory

distrib::
	@echo "xdg - desktop files --> $(DISTRIB_DIR)/isabelcore/env/usr/share/applications"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/env/usr/share/applications/isabel
	@for i in  $(DESKTOP_FILES) ; do \
	  echo "$$i  --> $(DISTRIB_DIR)/isabelcore/env/usr/share/applications/isabel"  ;\
	  cp -f $$i $(DISTRIB_DIR)/isabelcore/env/usr/share/applications/isabel/.  ;\
	done

distrib::
	@echo "xdg - menu files --> $(DISTRIB_DIR)/isabelcore/env/etc/xdg/menus/applications-merged"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/env/etc/xdg/menus/applications-merged
	@for i in  $(MENU_FILES) ; do \
	  echo "$$i  --> $(DISTRIB_DIR)/isabelcore/env/etc/xdg/menus/applications-merged"  ;\
	  cp -f $$i $(DISTRIB_DIR)/isabelcore/env/etc/xdg/menus/applications-merged ;\
	done

distrib::
	@echo "xdg menus --> $(DISTRIB_DIR)/isabelcore/env/usr/share/desktop-directories/isabel"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/env/usr/share/desktop-directories/isabel
	@for i in  $(DIRECTORY_FILES) ; do \
	  echo "$$i  --> $(DISTRIB_DIR)/isabelcore/env/usr/share/desktop-directories/isabel"  ;\
	  cp -f $$i $(DISTRIB_DIR)/isabelcore/env/usr/share/desktop-directories/isabel/.  ;\
	done

include $(PROJECT_ROOT_DIR)/Makefile.tail
