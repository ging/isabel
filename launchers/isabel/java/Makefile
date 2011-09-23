USE.LIBS= JARS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=launcher.jar

PACKAGES= \
   isabel.launcher

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard $(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)

ALLJARS:=$(TUNNELJAR):$(LIBJAR):$(GAPIJAR):$(GWSIPJAR):$(GWFLASHJAR):$(MJSIPJAR):.

$(OBJDIR)/launcher.jar: $(SRC) $(LIBJAR) $(TUNNELJAR) $(GAPIJAR) $(GWSIPJAR) $(GWFLASHJAR) $(MJSIPJAR)
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	javac -classpath $(ALLJARS) \
              -d $(OBJDIR) \
              isabel/launcher/Start.java
	cp MANIFEST.MF $(OBJDIR)/.
	cd $(OBJDIR) ; jar cfm launcher.jar MANIFEST.MF isabel


distrib::
	@echo "isabel_launcher.jar --> $(DISTRIB_DIR)/isabelcore/libexec"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/libexec
	@cp -f $(OBJDIR)/launcher.jar $(DISTRIB_DIR)/isabelcore/libexec/isabel_launcher.jar


javadoc:
	mkdir -p $(OBJDIR)/javadoc
	javadoc -private \
                   -classpath $(ALLJARS) \
                   -sourcepath . \
                   -d $(OBJDIR)/javadoc \
                   -link $(OBJS_ROOT_DIR)/lib/javacommon/javadoc \
                   -version \
                   -author \
                   $(PACKAGES)

clean::
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \; ; rm -rf $(OBJDIR)/javadoc

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

