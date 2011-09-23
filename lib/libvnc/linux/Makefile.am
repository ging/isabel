## Process this file with automake to produce Makefile.in
## Created by Anjuta

libvncdocdir = ${prefix}/doc/libvnc
libvncdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

EXTRA_DIST = $(libvncdoc_DATA)

lib_LIBRARIES = \
	libvnc.a

libvnc_a_SOURCES = \
	../src/d3des.cpp \
	../src/extras.cpp \
	../src/image.cpp \
	../src/jpeg.cpp \
	../src/locker.cpp \
	../src/rfb.cpp \
	../src/threadtask.cpp \
	../src/tight.cpp \
	../src/vncauth.cpp \
	../src/vncconn.cpp 

libvnc_a_CXXFLAGS = \
	-D__BUILD_FOR_LINUX\
	-I../include

lib_LTLIBRARIES = \
	libvnc.la

libvnc_la_SOURCES = \
	../src/d3des.cpp \
	../src/extras.cpp \
	../src/image.cpp \
	../src/jpeg.cpp \
	../src/locker.cpp \
	../src/rfb.cpp \
	../src/threadtask.cpp \
	../src/tight.cpp \
	../src/vncauth.cpp \
	../src/vncconn.cpp 

libvnc_la_LDFLAGS = \
        -ljpeg -z \
	-version-info \
	1:0:0 \
	-no-undefined


libvnc_la_CXXFLAGS = \
	-D__BUILD_FOR_LINUX \
	-I../include

libvnc_la_LIBADD= -ljpeg -lz
 
# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

