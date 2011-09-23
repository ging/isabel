USE.LIBS=JARS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=isabel_argonauta.jar

PACKAGES= \
   isabel.argonauta

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard $(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)

$(OBJDIR)/isabel_argonauta.jar: $(SRC) $(LIBJAR)
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	javac -classpath $(LIBJAR):. \
              -d $(OBJDIR) \
              isabel/argonauta/Argonauta.java
	cp manifest.txt $(OBJDIR)/.
	cd $(OBJDIR) ; jar cfm isabel_argonauta.jar manifest.txt isabel


distrib::
	@echo "isabel_argonauta.jar --> $(DISTRIB_DIR)/isabelcore/libexec"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/libexec
	@cp -f $(OBJDIR)/isabel_argonauta.jar $(DISTRIB_DIR)/isabelcore/libexec/isabel_argonauta.jar

clean::
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

