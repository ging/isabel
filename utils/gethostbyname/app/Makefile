include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=isabel_gethostbyname isabel_fqdn isabel_ipv6ready

SUBDIRS.PRE=
SUBDIRS.POST=


SRC1=gethostbyname.c
OBJ1=$(SRC1:%.c=$(OBJDIR)/%.o)

SRC2=fqdn.c
OBJ2=$(SRC2:%.c=$(OBJDIR)/%.o)

SRC3=ipv6ready.c
OBJ3=$(SRC3:%.c=$(OBJDIR)/%.o)


#
# $(LINK.ICF)     -- link icf
# $(LINK.SDK)     -- link sdk
# $(LINK.X11)     -- link x11
# $(LINK.THREADS) -- pthreads (and Solaris threads) (required by icf)
# $(LINK.STD)     -- math, sockets, name services and the like
#
$(OBJDIR)/isabel_gethostbynameold: $(OBJ0)
	$(LD) -o $@ $(OBJ0) 

$(OBJDIR)/isabel_gethostbyname: $(OBJ1)
	$(LD) -o $@ $(OBJ1) 

$(OBJDIR)/isabel_fqdn: $(OBJ2)
	$(LD) -o $@ $(OBJ2) 

$(OBJDIR)/isabel_ipv6ready: $(OBJ3)
	$(LD) -o $@ $(OBJ3) 

distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@echo "isabel_gethostbyname --> $(DISTRIB_DIR)/isabelcore/bin"
	@cp -f $(OBJDIR)/isabel_gethostbyname $(DISTRIB_DIR)/isabelcore/bin
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_gethostbyname
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_gethostbyname
	@echo "isabel_ipv6ready --> $(DISTRIB_DIR)/isabelcore/bin"
	@cp -f $(OBJDIR)/isabel_ipv6ready $(DISTRIB_DIR)/isabelcore/bin
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_ipv6ready
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_ipv6ready

NO_distrib::
	@mkdir -p $(DISTRIB_DIR)/isabelcore/bin
	@echo "isabel_fqdn --> $(DISTRIB_DIR)/isabelcore/bin"
	@cp -f $(OBJDIR)/isabel_fqdn $(DISTRIB_DIR)/isabelcore/bin
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/bin/isabel_fqdn
	@chmod a+x $(DISTRIB_DIR)/isabelcore/bin/isabel_fqdn

clean::
	rm -f $(OBJ1) $(OBJ2) $(OBJ3)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
