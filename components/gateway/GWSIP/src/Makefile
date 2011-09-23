USE.LIBS= JARS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=gwsip.jar

PACKAGES= \
   isabel.gwsip

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard $(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)

ISABEL_DIR=/usr/local/isabel
ISABELEXTRAS_DIR=/usr/local/isabelextras500

ALLJARS=$(GAPIJAR):$(MJSIPJAR):.

$(OBJDIR)/gwsip.jar: $(SRC) $(MJSIP) $(GAPIJAR)
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	javac -classpath $(ALLJARS) \
              -d $(OBJDIR) \
              isabel/gwsip/SipGateway.java
	cp manifest.txt $(OBJDIR)/.
	cd $(OBJDIR) ; jar cf gwsip.jar manifest.txt isabel


distrib::
	@echo "isabel_gwsip.jar --> $(DISTRIB_DIR)/isabelgw/libexec/isabel_gwsip.jar"
	@mkdir -p $(DISTRIB_DIR)/isabelgw/libexec
	@cp -f $(OBJDIR)/gwsip.jar $(DISTRIB_DIR)/isabelgw/libexec/isabel_gwsip.jar


javadoc:
	mkdir -p $(OBJDIR)/javadoc
	javadoc -private \
                   -classpath $(LIBJAR):. \
                   -sourcepath . \
                   -d $(OBJDIR)/javadoc \
                   -link $(OBJS_ROOT_DIR)/lib/javacommon/javadoc \
                   -version \
                   -author \
                   $(PACKAGES)

clean::
	cd $(OBJDIR) ; rm *.jar  ; rm -rf $(OBJDIR)/javadoc

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

