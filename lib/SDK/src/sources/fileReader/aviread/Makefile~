USE.LIBS= ICF2-THREADED SDK VCODECS
include $(PROJECT_ROOT_DIR)/Makefile.head

SUBDIRS.PRE=
SUBDIRS.POST=

TARGET= AviReadHandler.o \
	AviReadStream.o \
	BaseError.o \
	Cache.o \
	Locker.o \
	ReadStream.o \
	StreamInfo.o \
	utils.o \
	wave_type.o


SRC=	AviReadHandler.cpp \
	AviReadStream.cpp \
	BaseError.cpp \
	Cache.cpp \
	Locker.cpp \
	ReadStream.cpp \
	StreamInfo.cpp \
	utils.cpp \
	wave_type.cpp

OBJ=$(SRC:%.cpp=$(OBJDIR)/%.o)

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)

include $(PROJECT_ROOT_DIR)/Makefile.tail
