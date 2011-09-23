USE.LIBS=JARS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=isabel_rtp_echo.jar

PACKAGES= .

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard $(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)

$(OBJDIR)/isabel_rtp_echo.jar: $(SRC) $(LIBJAR)
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	javac -classpath $(LIBJAR):. \
              -d $(OBJDIR) \
              IsabelRtpEcho.java
	cd $(OBJDIR) ; jar cf isabel_rtp_echo.jar IsabelRtpEcho.class


distrib::
	@echo "isabel_rtp_echo.jar --> $(DISTRIB_DIR)/isabelcore/libexec"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/libexec
	@cp -f $(OBJDIR)/isabel_rtp_echo.jar $(DISTRIB_DIR)/isabelcore/libexec/isabel_rtp_echo.jar


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

