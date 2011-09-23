include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=alsamixer.jar

PACKAGES= \
   isabel.sam

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard $(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)


$(OBJDIR)/alsamixer.jar: $(SRC)
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	javac -classpath . \
              -d $(OBJDIR) \
              isabel/sam/AudioMixerDaemon.java
	javac -classpath . \
              -d $(OBJDIR) \
              isabel/sam/AudioConfigurer.java
	cd $(OBJDIR) ; jar cf $(TARGET) isabel


distrib::
	@echo "isabel_alsamixer.jar --> $(DISTRIB_DIR)/libexec"
	@cp -f $(OBJDIR)/$(TARGET) $(DISTRIB_DIR)/libexec/isabel_$(TARGET)
	@echo "sam.cfg --> $(DISTRIB_DIR)/lib"
	@cp -f sam.cfg $(DISTRIB_DIR)/lib/.

clean::
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

