ifeq ($(PROJECT_ROOT_DIR),) 
PROJECT_ROOT_DIR=$(shell pwd)
MAKEFLAGS+=PROJECT_ROOT_DIR=$(PROJECT_ROOT_DIR)
endif

include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE= lib utils
SUBDIRS.POST=components launchers images activities admin \
             doc xlim



#
# extra work after building all
#
# target::
# 	@echo Building binary distribution...
# 	@PROJECT_ROOT_DIR=$(PROJECT_ROOT_DIR) $(PROJECT_ROOT_DIR)/mkDistrib
# 	@echo Done... binary distribution is at $(PROJECT_ROOT_DIR)/nVideo.tgz


distrib::
	@find $(DISTRIB_DIR) -name .svn -prune -exec rm -rf {} \;
	@mkdir -p $(DISTRIB_DIR)/isabelcore
	@echo "CREDITS --> $(DISTRIB_DIR)/isabelcore/."
	@cp -f CREDITS $(DISTRIB_DIR)/isabelcore/.
	@echo "COPYING --> $(DISTRIB_DIR)/isabelcore/."
	@cp -f COPYING $(DISTRIB_DIR)/isabelcore/.

install::
	@export USER=`whoami`
	@if [ "$(USER)" != "root" ] ; then \
	    echo "ERROR: Installing ISABEL requires being root OR sudo permissions" ; \
            exit 1 ; \
	else \
            export DPKG_MAINTSCRIPT_PACKAGE=isabel000 \
            export PKG_DIR="/" ; \
            export ISA_RELDIR="usr/local/isabel000/" ; \
            export DISTRIB_DIR="$(DISTRIB_DIR)" ; \
	    cd $(PROJECT_ROOT_DIR)/admin/pkg.Isabel500 ; \
	    ./INSTALL-Linux ; \
	    cd $(PROJECT_ROOT_DIR)/admin/pkg.IsabelGw500 ; \
	    ./INSTALL-Linux ; \
	fi

uninstall::
	@export USER=`whoami`
	@if [ "$(USER)" != "root" ] ; then \
	    echo "ERROR: Uninstalling ISABEL requires being root OR sudo permissions" ; \
            exit 1 ; \
	else \
            test -d /usr/local/isabel000 && mv /usr/local/isabel000 /tmp/isabel000.$$$$ ; \
            update-alternatives --auto isabel ; \
            update-alternatives --remove isabel /usr/local/isabel000/bin/isabel ; \
            update-desktop-database || true ; \
            killall -q gnome-panel || true ; \
        fi

devel::
	@echo "Installed dev packages needed to compile isabel"
	@cd $(PROJECT_ROOT_DIR)/admin ; sudo ./install_dev_packages.sh


VERSION_FILE=$(PROJECT_ROOT_DIR)/admin/pkg.Isabel500/version
VERSION=$(shell $(PROJECT_ROOT_DIR)/admin/version.mgr getVersion $(VERSION_FILE))
RELEASE=$(shell $(PROJECT_ROOT_DIR)/admin/version.mgr getRelease $(VERSION_FILE))

package:
	@echo "BUILDING TEMPORAL ISABEL $(VERSION)-$(RELEASE) PACKAGE"
	@echo -n "WARNING: Be sure 'make' and 'make distrib' have been "
	@echo "already issued"
	@cd $(PROJECT_ROOT_DIR)/admin ; ./mkpkg Isabel500 -skipVersion -tmppkg


rmdist:
	@echo "   ...borrando $(DISTRIB_DIR)"
	rm -rf $(DISTRIB_DIR)

include $(PROJECT_ROOT_DIR)/Makefile.tail
# DO NOT DELETE
