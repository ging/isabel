include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=images help
SUBDIRS.POST=

TARGET=agent.jar

SRC=  $(wildcard *.java) \
      $(wildcard sck/*.java) \
      $(wildcard graph/*.java)
        
CLASSES=$(SRC:%.java=$(OBJDIR)/%.class)


$(OBJDIR)/agent.jar: $(SRC) $(OBJDIR)/manifest.txt
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \;
	javac -classpath . \
              -d $(OBJDIR) \
              $(SRC)
	cd $(OBJDIR) ; jar cfm agent.jar manifest.txt *.class sck/*.class graph/*.class images/* help/*


$(OBJDIR)/manifest.txt:	manifest.txt
	cp manifest.txt $(OBJDIR)/manifest.txt

distrib::
	@echo "isabel_SNMPAgent.jar --> $(DISTRIB_DIR)/isabelcore/libexec"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/libexec
	@cp -f $(OBJDIR)/agent.jar $(DISTRIB_DIR)/isabelcore/libexec/isabel_SNMPAgent.jar

clean::
	cd $(OBJDIR) ; find . -name "*.class" -exec rm {} \; ; rm -rf $(OBJDIR)/javadoc

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail

