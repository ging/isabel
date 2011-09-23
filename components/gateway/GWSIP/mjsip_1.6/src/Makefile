include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=mjsip.jar

PACKAGES= \
   local.media \
   local.net \
   local.server \
   local.ua \
   org.zoolu.net \
   org.zoolu.sdp \
   org.zoolu.sip.address \
   org.zoolu.sip.authentication \
   org.zoolu.sip.call \
   org.zoolu.sip.dialog \
   org.zoolu.sip.header \
   org.zoolu.sip.message \
   org.zoolu.sip.provider \
   org.zoolu.sip.transaction \
   org.zoolu.tools

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard $(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)

ISABEL_DIR=/usr/local/isabel
ISABELEXTRAS_DIR=/usr/local/isabelextras411

ALLJARS=.

$(OBJDIR)/mjsip.jar: $(SRC) $(LIBJAR) $(SECOJAR)
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	javac -classpath $(ALLJARS) \
              -d $(OBJDIR) \
              $(SRC)
	cd $(OBJDIR) ; jar cf mjsip.jar local org


distrib::
	@echo "isabel_mjsip.jar --> $(DISTRIB_DIR)/isabelgw/libexec/isabel_mjsip.jar"
	@mkdir -p $(DISTRIB_DIR)/isabelgw/libexec
	@cp -f $(OBJDIR)/mjsip.jar $(DISTRIB_DIR)/isabelgw/libexec/isabel_mjsip.jar


javadoc:
	mkdir -p $(OBJDIR)/javadoc
	javadoc -private \
                   -classpath $(OBJDIR.PREFIX)/$(OBJDIR.OS)/lib/javacommon/lib.jar:. \
                   -sourcepath . \
                   -d $(OBJDIR)/javadoc \
                   -link $(OBJDIR.PREFIX)/$(OBJDIR.OS)/lib/javacommon/javadoc \
                   -version \
                   -author \
                   $(PACKAGES)

clean::
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \; ; rm -rf $(OBJDIR)/javadoc

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

