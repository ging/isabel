USE.LIBS= ICF2-THREADED X11 THREADS VCODECS GWR
include $(PROJECT_ROOT_DIR)/Makefile.head

TARGET=	\
	libgwr.a	\
	libgwr.so

SRC=	\
	displayTask.cc	\
	gwrFactory.cc

ifeq ($(TARGET.OS),Linux)
SUBDIRS.PRE= X11 OpenGL GLX
SRC+= X11/bufferTranslations.cc X11/color.cc X11/dtX11.cc GLX/dtGLX.cc OpenGL/dtOpenGL.cc
endif
ifeq ($(TARGET.OS),Darwin)
SUBDIRS.PRE= X11 OpenGL GLX
SRC+= X11/bufferTranslations.cc X11/color.cc X11/dtX11.cc GLX/dtGLX.cc OpenGL/dtOpenGL.cc
endif
ifeq ($(TARGET.OS),Win32)
SUBDIRS.PRE= OpenGL GLUT
endif
ifeq ($(TARGET.OS),SunOS)
SUBDIRS.PRE= X11 OpenGL GLX
SRC+= X11/bufferTranslations.cc X11/color.cc X11/dtX11.cc GLX/dtGLX.cc OpenGL/dtOpenGL.cc
endif

SUBDIRS.POST=

GWR.Version:=$(shell cat ../version)


OBJ=$(SRC:%.cc=$(OBJDIR)/%.o)


$(OBJDIR)/libgwr.a: $(OBJ)
	ar cr $@ $?

$(OBJDIR)/libgwr.so: $(OBJ)
	 $(LD.SHARED) -o $@.$(GWR.Version) $(OBJ) $(LINK.ICF) $(LINK.X11) -lXext -lXmu $(LINK.THREADS) -lGL -lglut -lGLU $(LINK.STD) -Xlinker -soname=$$(basename $@).$(GWR.Version)
	 -rm -f $@
	 ln -s ./libgwr.so.$(GWR.Version) $@

distrib::
	@echo "libgwr.so -> $(DISTRIB_DIR)/isabelcore/lib"
	@mkdir -p $(DISTRIB_DIR)/isabelcore/lib
	@cp -f $(OBJDIR)/libgwr.so.* $(DISTRIB_DIR)/isabelcore/lib
	@$(STRIP) $(DISTRIB_DIR)/isabelcore/lib/libgwr.so.*
	@chmod 644 $(DISTRIB_DIR)/isabelcore/lib/libgwr.so.*
	@ln -sf libgwr.so.$(GWR.Version) $(DISTRIB_DIR)/isabelcore/lib/libgwr.so

clean::
	rm -f $(OBJ)

cleanall:: clean
	rm -f $(TARGET:%=$(OBJDIR)/%)
	rm -f $(OBJDIR)/libgwr.so*

include $(PROJECT_ROOT_DIR)/Makefile.tail
