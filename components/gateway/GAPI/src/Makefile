USE.LIBS= JARS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=gapi.jar

PACKAGES= \
   isabel.gw \
   isabel.gw.flows \
   isabel.gw.isabel_client \
   isabel.gw.isabel_client.components \
   isabel.gw.isabel_client.isabel_state \
   isabel.gw.isabel_client.simulator2


DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard $(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)

ISABEL_DIR=/usr/local/isabel
ISABELEXTRAS_DIR=/usr/local/isabelextras411

ALLJARS=$(LIBJAR):$(SECOJAR):.

$(OBJDIR)/gapi.jar: $(SRC) $(LIBJAR) $(SECOJAR)
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	javac -classpath $(ALLJARS) \
              -d $(OBJDIR) \
              isabel/gw/GAPIImplementation.java \
              isabel/gw/isabel_client/IsabelClientImpl.java \
              isabel/gw/isabel_client/simulator2/Simulator.java
	cp manifest.txt $(OBJDIR)/.
	cd $(OBJDIR) ; jar cf gapi.jar manifest.txt isabel


distrib::
	@echo "isabel_gapi.jar --> $(DISTRIB_DIR)/isabelgw/libexec/isabel_gapi.jar"
	@mkdir -p $(DISTRIB_DIR)/isabelgw/libexec
	@cp -f $(OBJDIR)/gapi.jar $(DISTRIB_DIR)/isabelgw/libexec/isabel_gapi.jar


javadoc:
	mkdir -p $(OBJDIR)/javadoc
	javadoc -private \
                   -classpath $(ALLJARS):. \
                   -sourcepath . \
                   -d $(OBJDIR)/javadoc \
                   -link $(OBJS_ROOT_DIR)/lib/javacommon/javadoc \
                   -version \
                   -author \
                   $(PACKAGES)

clean::
	cd $(OBJDIR) ; rm *.jar ; find . -name "*.class" -exec rm {} \; ; rm -rf $(OBJDIR)/javadoc

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

