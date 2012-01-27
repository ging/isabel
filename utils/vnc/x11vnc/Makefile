include $(PROJECT_ROOT_DIR)/Makefile.head

ISAX11VNC=isabel_x11vnc
ISAINET4TO6=isabel_inet4to6

TARGET=$(ISAX11VNC) $(ISAINET4TO6)


SUBDIRS.PRE=
SUBDIRS.POST=

PKGNAME=x11vnc-0.9.14
TARFILE=$(PKGNAME)-dev.tar.gz

$(OBJDIR)/$(ISAX11VNC): $(OBJDIR)/$(PKGNAME)/x11vnc/x11vnc
	cp $(OBJDIR)/$(PKGNAME)/x11vnc/x11vnc $(OBJDIR)/$(ISAX11VNC)

$(OBJDIR)/$(ISAINET4TO6): $(OBJDIR)/$(PKGNAME)/x11vnc/misc/inet4to6
	cp $(OBJDIR)/$(PKGNAME)/x11vnc/misc/inet6to4 $(OBJDIR)/$(ISAINET4TO6)

$(OBJDIR)/$(PKGNAME)/x11vnc: $(TARFILE)
	cp $(TARFILE) $(OBJDIR)/.
	cd $(OBJDIR) ; ( gzip -dc $(TARFILE) | tar -xvf - )

$(OBJDIR)/$(PKGNAME)/x11vnc/x11vnc: $(OBJDIR)/$(PKGNAME)/x11vnc
	cd $(OBJDIR)/$(PKGNAME) ; ./configure
	cd $(OBJDIR)/$(PKGNAME) ; make

$(OBJDIR)/$(PKGNAME)/x11vnc/misc/inet4to6: $(OBJDIR)/$(PKGNAME)/x11vnc

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin

distrib::
	@echo "$(ISAX111VNC) --> $(DISTRIB_DIR)/isabelcore/bin"
	@chmod +x $(OBJDIR)/$(ISAX11VNC)
	@cp -f $(OBJDIR)/$(ISAX11VNC) $(DISTRIB_DIR)/isabelcore/bin/.

distrib::
	@echo "$(ISAINET4TO6) --> $(DISTRIB_DIR)/isabelcore/bin"
	@chmod +x $(OBJDIR)/$(ISAINET4TO6)
	@cp -f $(OBJDIR)/$(ISAINET4TO6) $(DISTRIB_DIR)/isabelcore/bin/.

clean::
	rm -f $(OBJDIR)/$(ISAX11VNC)
	rm -f $(OBJDIR)/$(ISAINET4TO6)
	rm -f $(OBJDIR)/$(TARFILE)
	rm -rf $(OBJDIR)/$(PKGNAME)

cleanall:: clean
	rm -f $(ISAX11VNC:%=$(DISTRIB_DIR)/isabelcore/bin/%)
	rm -f $(ISAINET4TO6:%=$(DISTRIB_DIR)/isabelcore/bin/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
