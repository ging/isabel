USE.LIBS= JARS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=gwflash.jar

PACKAGES= \
   isabel.gwflash \
   isabel.gwflash.red5client \
   isabel.gwflash.red5client.oldred5 \
   isabel.gwflash.red5client.utils \
   isabel.igateway \
   isabel.gwflash.xuggle

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard $(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)

XUGGLE_HOME=/usr/local/xuggler
ISABEL_DIR=/usr/local/isabel
ISABELEXTRAS_DIR=/usr/local/isabelextras500
FLASHJAR=../lib/red5.jar
XUGGLEJAR=../lib/xuggle-xuggler.jar:../lib/commons-cli.jar
ALLJARS=$(LIBJAR):$(GAPIJAR):$(MJSIPJAR):$(FLASHJAR):$(XUGGLEJAR):.

$(OBJDIR)/gwflash.jar: $(SRC) $(MJSIP) $(GAPIJAR)
	echo "Makeando..."
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	javac -classpath $(ALLJARS) \
              -d $(OBJDIR) \
              isabel/gwflash/FlashGateway.java
	cp manifest.txt $(OBJDIR)/.
	cd $(OBJDIR) ; jar cf gwflash.jar manifest.txt isabel


distrib::
	@echo "isabel_gwflash.jar --> $(DISTRIB_DIR)/isabelgw/libexec/isabel_gwflash.jar"
	@mkdir -p $(DISTRIB_DIR)/isabelgw/libexec
	@cp -f $(OBJDIR)/gwflash.jar $(DISTRIB_DIR)/isabelgw/libexec/isabel_gwflash.jar

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
	cd $(OBJDIR) ; rm *.jar ; rm -rf $(OBJDIR)/javadoc

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

