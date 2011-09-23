USE.LIBS=JARS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=seco.jar

PACKAGES= \
   isabel.seco.server \
   isabel.seco.access \
   isabel.seco.network \
   isabel.seco.network.javaser \
   isabel.seco.network.tclser \
   isabel.seco.dispatcher \
   isabel.seco.dispatcher.javaser \
   isabel.seco.dispatcher.tclser

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard $(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)

$(OBJDIR)/seco.jar: $(SRC) $(LIBJAR)
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	javac -classpath $(LIBJAR):. \
              -d $(OBJDIR) \
              isabel/seco/server/SeCo.java \
              isabel/seco/network/Network.java \
              isabel/seco/network/javaser/JavaMarshaller.java \
              isabel/seco/network/tclser/TclMarshaller.java \
              isabel/seco/dispatcher/javaser/JavaDispatcher.java \
              isabel/seco/dispatcher/tclser/TclDispatcher.java
	cd $(OBJDIR) ; jar cf seco.jar isabel


distrib::
	@echo "isabel_seco.jar --> $(DISTRIB_DIR)/isabelcore/libexec"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/libexec
	@cp -f $(OBJDIR)/seco.jar $(DISTRIB_DIR)/isabelcore/libexec/isabel_seco.jar


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
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \; ; rm -rf $(OBJDIR)/javadoc

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

