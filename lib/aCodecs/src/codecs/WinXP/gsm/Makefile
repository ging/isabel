USE.LIBS= ACODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET=libgsm.a 

#Microsoft gsm 6.10
#uncomment the following to send two gsm packets with 65 bytes,
#rather than two gsm packets with 33 bytes
#CXXFLAGS+= -DWAV49

SRC= add.cxx\
     decode.cxx\
     gsm_destroy.cxx\
     long_term.cxx\
     code.cxx\
     gsm.cxx\
     gsm_encode.cxx\
     lpc.cxx\
     rpe.cxx\
     gsm_create.cxx\
     short_term.cxx\
     gsm_decode.cxx\
     gsm_option.cxx\
     preprocess.cxx\
     table.cxx

OBJ=$(SRC:%.cxx=$(OBJDIR)/%.o)


$(OBJDIR)/libgsm.a: $(OBJ)
	ar cr $@ $?


clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)
	rm -f $(OBJDIR)/libigsm.a

include $(PROJECT_ROOT_DIR)/Makefile.tail
