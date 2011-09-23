USE.LIBS=JARS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=tunnel.jar

PACKAGES= \
   isabel.tunnel

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard $(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)

$(OBJDIR)/tunnel.jar: $(SRC)
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	javac -classpath $(LIBJAR):. \
              -d $(OBJDIR) \
              isabel/tunnel/ClientTunnel.java \
              isabel/tunnel/ServerTunnel.java \
	      isabel/tunnel/TunnelUtils.java \
	      isabel/tunnel/Tunnelator.java \
	      isabel/tunnel/TunnelHandler.java \
	      isabel/tunnel/ActivePortTester.java \
	      isabel/tunnel/ActivePortTesterLauncher.java
	cp manifest.txt $(OBJDIR)/.
	cd $(OBJDIR) ; jar cfm tunnel.jar manifest.txt isabel


distrib::
	@echo "isabel_tunnel.jar --> $(DISTRIB_DIR)/isabelcore/libexec"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/libexec
	@cp -f $(OBJDIR)/tunnel.jar $(DISTRIB_DIR)/isabelcore/libexec/isabel_tunnel.jar

clean::
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)


include $(PROJECT_ROOT_DIR)/Makefile.tail

