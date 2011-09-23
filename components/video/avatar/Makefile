USE.LIBS=JARS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=avatar.jar

PACKAGES= \
   isabel.avatar

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard $(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)

$(OBJDIR)/avatar.jar: $(SRC) $(LIBJAR)
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	javac -classpath $(LIBJAR):. \
              -d $(OBJDIR) \
              isabel/avatar/MakeAvatar.java
	cd $(OBJDIR) ; jar cf avatar.jar isabel


distrib::
	@echo "isabel_avatar.jar --> $(DISTRIB_DIR)/isabelcore/libexec"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/libexec
	@cp -f $(OBJDIR)/avatar.jar $(DISTRIB_DIR)/isabelcore/libexec/isabel_avatar.jar


clean::
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

