USE.LIBS= JARS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=options scripts xdg	
SUBDIRS.POST=

TARGET=xlimservices.jar

PACKAGES= \
   services.isabel.lib \
   services.isabel.services.startsessionserver \
   services.isabel.services.guis \
   services.isabel.services.about \
   services.isabel.services.connect_to_lite \
   services.isabel.services.options \
   services.isabel.services.ac 

DIRPKGS := $(subst .,/,$(PACKAGES))
SRC := $(foreach PKG,$(DIRPKGS),$(wildcard ../$(PKG)/*.java))

CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)


ALLJARS=$(TUNNELJAR):$(XEDLJAR):$(XLIMJAR):$(LIBJAR):$(JAXBJAR):$(XMLRPCJARS):$(MAILJAR)

$(OBJDIR)/xlimservices.jar: $(SRC)
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	cd ..; javac -classpath $(ALLJARS):. \
              -d $(OBJDIR) \
              services/isabel/lib/Agenda.java \
              services/isabel/lib/Contacts.java \
              services/isabel/lib/ContactsManager.java \
              services/isabel/lib/Constants.java \
              services/isabel/lib/Crypt.java \
              services/isabel/lib/DesEncrypter.java \
              services/isabel/lib/ErrorPanel.java \
              services/isabel/lib/FileGarbageCollector.java \
              services/isabel/lib/LogStdErr.java \
              services/isabel/lib/LogStdOut.java \
              services/isabel/lib/NewContactPanel.java \
              services/isabel/lib/Servutilities.java \
              services/isabel/lib/StatusPanel.java \
              services/isabel/lib/StatusFrame.java \
              services/isabel/lib/StatusWindow.java \
              services/isabel/lib/XlimNotification.java \
              services/isabel/lib/XmlFilter.java \
              services/isabel/lib/ConnectionsManager.java \
              services/isabel/services/about/About.java \
              services/isabel/services/about/Aboutserver.java \
              services/isabel/services/guis/ServiceDefinitions.java \
              services/isabel/services/guis/XlimServicePanel.java \
              services/isabel/services/guis/ErrorPanel.java\
              services/isabel/services/guis/CheckParticipantsPanel.java \
              services/isabel/services/guis/EditSessionPanel.java \
              services/isabel/services/guis/InfoPanel.java \
              services/isabel/services/guis/SessionDataPanel_connect_to.java \
              services/isabel/services/guis/SessionDataPanel_start_server.java \
              services/isabel/services/guis/SiteDataPanel.java \
              services/isabel/services/connect_to_lite/Isabel_connect_to.java \
              services/isabel/services/startsessionserver/StartSessionServer.java \
              services/isabel/services/options/Options.java \
              services/isabel/services/options/OptionsPanelListener.java \
              services/isabel/services/options/AditionalParametersPanel.java \
              services/isabel/services/options/AdministrativeInfoPanel.java \
              services/isabel/services/options/AdvancedOnlyOptionsPanel.java \
              services/isabel/services/options/AdvancedOptionsPanel.java \
              services/isabel/services/options/BandwidthPanel.java \
              services/isabel/services/options/ConnectionModePanel.java \
              services/isabel/services/options/ConnectionModePanelListener.java \
              services/isabel/services/options/ErrorProtectionPanel.java \
              services/isabel/services/options/InputParameterPanel.java \
              services/isabel/services/options/IPAddressPanel.java \
              services/isabel/services/options/MediaControlsPanel.java \
              services/isabel/services/options/MulticastGroupsPanel.java \
              services/isabel/services/options/MulticastPanel.java \
              services/isabel/services/options/NetworkAccessPanel.java \
              services/isabel/services/options/OptionsStrings.java \
              services/isabel/services/options/OptionsTabs.java \
              services/isabel/services/options/ParityPanel.java \
              services/isabel/services/options/PublicNamePanel.java \
              services/isabel/services/options/RolePanel.java \
              services/isabel/services/options/SiteIDPanel.java \
              services/isabel/services/options/ValidablePanel.java \
              services/isabel/services/options/XEDLSiteDataPanel.java \
              services/isabel/services/ac/ServiceNames.java \
              services/isabel/services/ac/AcceptConnectionsStrings.java 
	cd $(OBJDIR) ; jar cf xlimservices.jar services 


distrib::
	@echo "isabel_xlimservices.jar --> $(DISTRIB_DIR)/isabelcore/libexec"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/libexec
	@cp -f $(OBJDIR)/xlimservices.jar $(DISTRIB_DIR)/isabelcore/libexec/isabel_xlimservices.jar

clean::
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

