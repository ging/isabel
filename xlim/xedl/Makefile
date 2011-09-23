USE.LIBS= JARS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=styles 
SUBDIRS.POST=

TARGET=xedl.jar

PACKAGES= \
   xedl.lib.jaxb \
   xedl.lib.xedl \
   xedl.lib.XEDLsplitter \
   xedl.lib.xerl \
   xedl.lib.xpathParser

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard ../$(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)


ALLJARS=$(TUNNELJAR):$(LIBJAR):$(JAXBJAR)

$(OBJDIR)/xedl.jar: $(SRC)
	echo ${OBJDIR}
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	cd .. ; javac -classpath $(ALLJARS):. \
              -d $(OBJDIR) \
              xedl/lib/jaxb/CONNECTIONMODE.java \
              xedl/lib/jaxb/ConnectionParameters.java \
              xedl/lib/jaxb/Edl.java \
              xedl/lib/jaxb/IpsecAction.java \
              xedl/lib/jaxb/IPSECCONF.java \
              xedl/lib/jaxb/IpsecDirection.java \
              xedl/lib/jaxb/IpsecExchangemode.java \
              xedl/lib/jaxb/IpsecProtocol.java \
              xedl/lib/jaxb/ISDN.java \
              xedl/lib/jaxb/ISDNCalled.java \
              xedl/lib/jaxb/ISDNCaller.java \
              xedl/lib/jaxb/ISDNParameters.java \
              xedl/lib/jaxb/IsdnRoleType.java \
              xedl/lib/jaxb/McastModeType.java \
              xedl/lib/jaxb/Mcu.java \
              xedl/lib/jaxb/MediaControl.java \
              xedl/lib/jaxb/Modem.java \
              xedl/lib/jaxb/ModemElement.java \
              xedl/lib/jaxb/Multicast.java \
              xedl/lib/jaxb/MulticastParameters.java \
              xedl/lib/jaxb/NETWORKACCESS.java \
              xedl/lib/jaxb/ObjectFactory.java \
              xedl/lib/jaxb/PersonalInformation.java \
              xedl/lib/jaxb/ProtectionType.java \
              xedl/lib/jaxb/SERVICE.java \
              xedl/lib/jaxb/SESSION.java \
              xedl/lib/jaxb/SESSIONADDITIONALPARAMS.java \
              xedl/lib/jaxb/SessionInfo.java \
              xedl/lib/jaxb/SITE.java \
              xedl/lib/jaxb/SITEADDITIONALPARAMS.java \
              xedl/lib/jaxb/SiteDescription.java \
              xedl/lib/jaxb/SiteIdentification.java \
              xedl/lib/jaxb/TEMPLATE.java \
              xedl/lib/jaxb/TemplateRoleType.java \
              xedl/lib/jaxb/Unicast.java \
              xedl/lib/jaxb/Upanddownbandwidth.java \
              xedl/lib/xedl/XEDL.java \
              xedl/lib/xedl/XEDLMixerTools.java \
	      xedl/lib/xedl/XEDLTools.java \
              xedl/lib/xedl/SiteList.java \
              xedl/lib/xedl/XEDLException.java \
	      xedl/lib/xedl/SmallXEDLTree.java \
	      xedl/lib/xedl/XEDLManager.java \
              xedl/lib/xedl/JAXBSingleton.java \
              xedl/lib/xedl/XEDLMixerException.java \
              xedl/lib/xerl/XERLException.java \
              xedl/lib/xerl/XERL.java \
              xedl/lib/XEDLsplitter/JavaParser.java \
              xedl/lib/XEDLsplitter/XEDLsplitter.java \
              xedl/lib/xpathParser/XPathParser.java
	cd $(OBJDIR) ; jar cf xedl.jar xedl

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/xlimconfig
	@echo "priorxedl.cfg    -->     $(DISTRIB_DIR)/isabelcore/lib/xlimconfig"
	@cp -f config/priorxedl.cfg            $(DISTRIB_DIR)/isabelcore/lib/xlimconfig/.
	@find $(DISTRIB_DIR)/isabelcore/lib/xlimconfig -name .svn -prune -exec rm -rf {} \;
	@echo "xedl.jar --> $(DISTRIB_DIR)/isabelcore/libexec"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/libexec
	@cp -f $(OBJDIR)/xedl.jar $(DISTRIB_DIR)/isabelcore/libexec/xedl.jar

clean::
	rm -rf $(DISTRIB_DIR)/isabelcore/lib/xlimconfig
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

