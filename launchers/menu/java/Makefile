USE.LIBS= JARS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=menu.jar

PACKAGES= \
   isabel.menulauncher 

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard ../$(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)

ALLJARS=$(TUNNELJAR):$(XMLRPCJARS):$(LIBJAR):$(XEDLJAR)

$(OBJDIR)/menu.jar: $(SRC)
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	echo $(JARS)	
	javac -classpath $(ALLJARS):. \
              -d $(OBJDIR) \
              isabel/menulauncher/IsabelMenu.java
	cd $(OBJDIR) ; jar cf menu.jar isabel 


distrib::
	@echo "isabel_menu.jar --> $(DISTRIB_DIR)/isabelcore/libexec"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/libexec
	@cp -f $(OBJDIR)/menu.jar $(DISTRIB_DIR)/isabelcore/libexec/isabel_menu.jar

clean::
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

