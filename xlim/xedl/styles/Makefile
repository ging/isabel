include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@echo "Styles --> $(DISTRIB_DIR)/isabelcore/lib/xedlsplitter"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/xedlsplitter
	@cp -f xedl2part.xsl $(DISTRIB_DIR)/isabelcore/lib/xedlsplitter/.
	@cp -f xedl2site.xsl $(DISTRIB_DIR)/isabelcore/lib/xedlsplitter/.
	@cp -f edl1-8.xsd $(DISTRIB_DIR)/isabelcore/lib/xedlsplitter/.
	@echo "xml2isa.xsl --> $(DISTRIB_DIR)/isabelcore/lib"
	@cp -f xml2isa.xsl $(DISTRIB_DIR)/isabelcore/lib/.        

cleanall:: clean
	rm -f $(DISTRIB_DIR)/isabelcore/lib/xml2isa.xsl

include $(PROJECT_ROOT_DIR)/Makefile.tail

