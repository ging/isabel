include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=seco_tests.jar

PACKAGES= \
   isabel.seco.tests.test1 \
   isabel.seco.tests.test2 \
   isabel.seco.tests.test3 \
   isabel.seco.tests.test5

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard $(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)

LIBJAR=$(OBJDIR.PREFIX)/$(OBJDIR.OS)/lib/javacommon/lib.jar
SECOJAR=$(OBJDIR)/../app4/seco.jar

$(OBJDIR)/seco_tests.jar: $(SRC) $(LIBJAR) $(SECOJAR)
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	javac -classpath $(LIBJAR):$(SECOJAR):. \
              -d $(OBJDIR) \
              isabel/seco/tests/test1/Aplicacion1.java \
	      isabel/seco/tests/test1/Aplicacion2.java \
	      isabel/seco/tests/test2/StartAplications.java \
	      isabel/seco/tests/test3/StartAplications.java \
	      isabel/seco/tests/test5/PruebaReceptor.java \
	      isabel/seco/tests/test5/PruebaCliente.java \
	      isabel/seco/tests/test5/Aplicacion5.java \
	      isabel/seco/tests/test5/PruebaClienteSincronismo.java \
	      isabel/seco/tests/test5/PruebaReceptorSincronismo.java
	cd $(OBJDIR) ; jar cf seco_tests.jar isabel


distrib::
	@echo "isabel_seco_tests.jar --> $(DISTRIB_DIR)/libexec"
	@cp -f $(OBJDIR)/seco_tests.jar $(DISTRIB_DIR)/libexec/isabel_seco_tests.jar


javadoc:
	mkdir -p $(OBJDIR)/javadoc
	javadoc -private \
                   -classpath $(LIBJAR):$(SECOJAR):. \
                   -sourcepath . \
                   -d $(OBJDIR)/javadoc \
                   -link $(OBJDIR.PREFIX)/$(OBJDIR.OS)/lib/javacommon/javadoc \
                   -version \
                   -author \
                   $(PACKAGES)

clean::
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \; ; rm -rf $(OBJDIR)/javadoc

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

