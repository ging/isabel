include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=app java 
SUBDIRS.POST=

CONF_FILES:=client_basic.conf \
            server_basic.conf \
	    ports \
	    client_timeout_correction_factor 


KEY_FILES:=ca.crt \
           ca.key \
           cliente.crt \
           cliente.csr \
           cliente.key \
           dh1024.pem \
           server.crt \
           server.key

SCRIPTS2:=startClientTunnel.sh \
          startServerTunnel.sh \



distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/tunnel/conf
	@for i in $(CONF_FILES) ; do \
	  echo "$$i     --> $(DISTRIB_DIR)/isabelcore/lib/tunnel/conf/." ;\
	  cp -f conf/$$i $(DISTRIB_DIR)/isabelcore/lib/tunnel/conf/. ;\
	done
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib/tunnel/keys
	@for i in $(KEY_FILES) ; do \
	  echo "$$i     --> $(DISTRIB_DIR)/isabelcore/lib/tunnel/keys" ;\
	  cp -rf keys/$$i $(DISTRIB_DIR)/isabelcore/lib/tunnel/keys/. ;\
	done
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@for i in $(SCRIPTS2) ; do \
	  echo "$$i     --> $(DISTRIB_DIR)/isabelcore/bin" ;\
	  chmod a+r scripts/$$i ;\
	  cp -f scripts/$$i $(DISTRIB_DIR)/isabelcore/bin/. ;\
	done
	@echo "isabel_openvpn --> $(DISTRIB_DIR)/isabelcore/bin"
	@cp -f $(OBJDIR)/app/isabel_openvpn $(DISTRIB_DIR)/isabelcore/bin/.


cleanall:: clean
	rm -rf $(DISTRIB_DIR)/lib/tunnel

include $(PROJECT_ROOT_DIR)/Makefile.tail

