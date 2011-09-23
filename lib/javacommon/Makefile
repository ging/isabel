include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=lib
SUBDIRS.POST=

TARGET=lib.jar

PACKAGES= \
   isabel.lib \
   isabel.lib.tasksock \
   isabel.lib.tasks \
   isabel.lib.reliableudp

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard $(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)

JARS=$(OBJDIR.PREFIX)/$(OBJDIR.OS)/lib/xedl/java/xedl.jar:$(OBJDIR.PREFIX)/$(OBJDIR.OS)/lib/xerl/java/xerl.jar

$(OBJDIR)/lib.jar: $(SRC) isabel/lib/colores
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	javac -classpath $(JARS):. -d $(OBJDIR) $(SRC)
	cp isabel/lib/colores $(OBJDIR)/isabel/lib/.
	cd $(OBJDIR) ; jar cf lib.jar isabel


distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/libexec
	@echo "isabel_lib.jar --> $(DISTRIB_DIR)/isabelcore/libexec"
	@cp -f $(OBJDIR)/lib.jar $(DISTRIB_DIR)/isabelcore/libexec/isabel_lib.jar


javadoc:
	mkdir -p $(OBJDIR)/javadoc
	javadoc -private \
                   -sourcepath . \
                   -d $(OBJDIR)/javadoc \
                   -version \
                   -author \
                   $(PACKAGES)

clean::
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

