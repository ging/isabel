USE.LIBS= ACODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET= codebook.o   \
        dmain.o      \
        postfil.o    \
        filters.o    \
        global.o     \
        adapters.o   \
        data.o       \
        io-sparc.o   \
        cmain.o

SRC=    codebook.cxx   \
        dmain.cxx      \
        postfil.cxx    \
        filters.cxx    \
        global.cxx     \
        adapters.cxx   \
        data.cxx       \
        io-sparc.cxx   \
        cmain.cxx

OBJ=$(SRC:%.cxx=$(OBJDIR)/%.o)

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
