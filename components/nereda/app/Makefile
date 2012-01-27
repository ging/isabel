USE.LIBS=JARS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=nereda.jar

PACKAGES= \
   isabel.nereda

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard $(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)

$(OBJDIR)/nereda.jar: $(SRC) $(LIBJAR)
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	javac -classpath $(LIBJAR):. \
              -d $(OBJDIR) \
              isabel/nereda/NeReDa.java
	cp manifest.txt $(OBJDIR)/.
	cd $(OBJDIR) ; jar cfm nereda.jar manifest.txt isabel


distrib::
	@echo "isabel_nereda.jar --> $(DISTRIB_DIR)/isabelcore/libexec"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/libexec
	@cp -f $(OBJDIR)/nereda.jar $(DISTRIB_DIR)/isabelcore/libexec/isabel_nereda.jar

clean::
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)


include $(PROJECT_ROOT_DIR)/Makefile.tail

