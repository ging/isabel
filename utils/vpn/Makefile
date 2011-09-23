include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

distrib::
	@echo "Scripts --> $(DISTRIB_DIR)/isabelcore/bin"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	chmod a+x edl2ipsec.sh getcert.sh
	@cp -f edl2ipsec.sh $(DISTRIB_DIR)/isabelcore/bin/vpn_edl2ipsec.sh
	@cp -f getcert.sh   $(DISTRIB_DIR)/isabelcore/bin/vpn_getcert.sh
	@echo "Styles --> $(DISTRIB_DIR)/isabelcore/lib/vpn"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/vpn
	@cp -f get_identity.xsl   $(DISTRIB_DIR)/isabelcore/lib/vpn/.
	@cp -f get_mcu_master.xsl $(DISTRIB_DIR)/isabelcore/lib/vpn/.
	@cp -f phase1.xsl         $(DISTRIB_DIR)/isabelcore/lib/vpn/.
	@cp -f phase2.xsl         $(DISTRIB_DIR)/isabelcore/lib/vpn/.
	@cp -f phase3.xsl         $(DISTRIB_DIR)/isabelcore/lib/vpn/.

cleanall:: clean
	rm -f $(DISTRIB_DIR)/lib/vpn/*
	rm -f vpn_edl2ipsec.sh vpn_getcert.sh

include $(PROJECT_ROOT_DIR)/Makefile.tail

