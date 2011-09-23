ifeq ($(ISABEL_HEADERS),)
ISABEL_HEADERS=DoSetYour_ISABEL_HEADERS_environmentVariable
endif

CXX = g++
CXXFLAGS= $(CXXFLAGS.include) $(CXXFLAGS.define) $(CXXFLAGS.optimize)

CXXFLAGS.include  = -I $(ISABEL_HEADERS)
CXXFLAGS.define   = -D__DEBUG -D__DEBUG_ICF -D__THREADED_ICF -D__BUILD_FOR_INTEL_LINUX
CXXFLAGS.optimize = -O2
CXXFLAGS.options  = -fpic -fPIC


LD = $(CXX)
LDFLAGS = $(LDFLAGS.libdirs) $(LDFLAGS.libraries) $(LDFLAGS.options)

LDFLAGS.libdirs   = -L/usr/lib
LDFLAGS.libraries = -licfsdk -lpthread -lm
LDFLAGS.options   = $(CXXFLAGS.options)

SOURCES:=$(wildcard *.cc)
TARGET :=$(SOURCES:%.cc=%)


target: $(TARGET)


clean:
	-rm -f $(TARGET)

anew: clean target

