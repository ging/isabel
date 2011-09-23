USE.LIBS= ICF2-NON-THREADED
include $(PROJECT_ROOT_DIR)/Makefile.head

ifeq ($(TARGET.OS),Darwin)
TARGET=libnonthreadedicf.a libnonthreadedicf.dylib
else
TARGET=libnonthreadedicf.a libnonthreadedicf.so
endif

FILES=\
	notify.cc item.cc smartReference.cc	\
	ql.cc vector.cc dictionary.cc	\
	io.cc fileIO.cc sockIO.cc	\
	lockSupport.cc conditionalVar.cc threadedLoop.cc	\
	icfMessages.cc task.cc sched.cc	\
	parseCmdLine.cc	\
	stdTask.cc	\
	strOps.cc

SRC=$(FILES:%.cc=../src/%.cc)
OBJ=$(FILES:%.cc=$(OBJDIR)/%.o)


$(OBJDIR)/%.o:	../src/%.cc
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

# Version number need to be changed whenever:
#
#   i) New virtual methods are added to current classes
#  ii) New attributes are added to current classes
#
# It is adequated (but not mandatory) whenever:
#
#    i) New classes are added
#   ii) New (non virtual) methods are added to current classes
#  iii) New 'static' are added
#
# It is foolish to do it whenever:
#
#   i) A method implementation is changed
#  ii) Forgotten traces are removed :)

ICF.Version:=$(shell cat ../version)


$(OBJDIR)/libnonthreadedicf.a: $(OBJ)
	ar cr $@ $?

$(OBJDIR)/libnonthreadedicf.so: $(OBJ)
	$(LD.SHARED) -o $@.$(ICF.Version) $(OBJ) -Xlinker -soname=$$(basename $@).$(ICF.Version)
	-rm -f $@
	ln -s ./libnonthreadedicf.so.$(ICF.Version) $@

$(OBJDIR)/libnonthreadedicf.dylib: $(OBJ)
	$(LD.SHARED) -compatibility_version $(ICF.Version) -current_version $(ICF.Version) -o $(OBJDIR)/libnonthreadedicf.$(ICF.Version).dylib $(OBJ)
	-rm -f $@
	ln -s ./libnonthreadedicf.$(ICF.Version).dylib $@

distrib::
ifeq ($(TARGET.OS),Darwin)
	@echo "libnonthreadedicf.dylib --> $(DISTRIB_DIR)/isabelcore/lib"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@cp -f $(OBJDIR)/libnonthreadedicf.*dylib $(DISTRIB_DIR)/isabelcore/lib
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/lib/libnonthreadedicf.*dylib
else
	@echo "libnonthreadedicf.so --> $(DISTRIB_DIR)/isabelcore/lib"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@cp -f $(OBJDIR)/libnonthreadedicf.so.* $(DISTRIB_DIR)/isabelcore/lib
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/lib/libnonthreadedicf.so.*
	@ln -sf libnonthreadedicf.so.$(ICF.Version) $(DISTRIB_DIR)/isabelcore/lib/libnonthreadedicf.so 
endif

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)
	rm -f $(OBJDIR)/libnonthreadedicf.*

include $(PROJECT_ROOT_DIR)/Makefile.tail
