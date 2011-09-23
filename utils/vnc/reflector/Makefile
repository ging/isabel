# VNC Reflector
# Copyright (C) 2001-2003 HorizonLive.com, Inc.  All rights reserved.
#
# This software is released under the terms specified in the file LICENSE,
# included.  HorizonLive provides e-Learning and collaborative synchronous
# presentation solutions in a totally Web-based environment.  For more
# information about HorizonLive, please see our website at
# http://www.horizonlive.com.
#
# This software was authored by Constantin Kaplinsky <const@ce.cctpu.edu.ru>
# and sponsored by HorizonLive.com, Inc.
#
# $Id: Makefile 22251 2011-04-12 10:37:44Z gabriel $
#
# Variables you might want to edit: CFLAGS, CONFFLAGS

IFLAGS =	-I.

# Production
CFLAGS =	-O2 $(IFLAGS) 
# Debug (strict)
#CFLAGS =	-g -pedantic -Wall $(IFLAGS)
# Debug (profiling)
#CFLAGS =	-g -pg $(IFLAGS)
# Debug (normal)
#CFLAGS =	-g $(IFLAGS)

# Use poll(2) syscall in async I/O instead of select(2)
CONFFLAGS =	-DUSE_POLL

TARGET.OS :=$(shell uname)

# Link with zlib and JPEG libraries
ifeq ($(TARGET.OS),Darwin)
JPEG_INCDIR = -I/opt/local/include
LDFLAGS = -L/opt/local/lib -ljpeg -lz
else
LDFLAGS = -ljpeg -lz
endif

PROG = 	isabel_vncreflector

OBJS = 	main.o logging.o active.o actions.o host_connect.o d3des.o rfblib.o \
	async_io.o host_io.o client_io.o encode.o region.o translate.o \
	control.o encode_tight.o decode_hextile.o decode_tight.o fbs_files.o \
	region_more.o

SRCS =	main.c logging.c active.c actions.c host_connect.c d3des.c rfblib.c \
	async_io.c host_io.c client_io.c encode.c region.c translate.c \
	control.c encode_tight.c decode_hextile.c decode_tight.c fbs_files.c \
	region_more.c

CC = gcc
MAKEDEPEND = makedepend
MAKEDEPFLAGS = -Y

all: $(PROG)

default: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS) $(LDFLAGS)

clean: 
	rm -f $(OBJS) *core* ./*~ ./*.bak $(PROG)

depend: $(SRCS)
	$(MAKEDEPEND) $(MAKEDEPFLAGS) -I. $(SRCS) 2> /dev/null

.c.o:
	$(CC) $(CFLAGS) $(JPEG_INCDIR) $(CONFFLAGS) -c $<

install:
	cp IPv6_vncreflector /tmp/

unistall:
	rm /tmp/IPv6_vncreflector
 
# DO NOT DELETE

main.o: rfblib.h async_io.h logging.h reflector.h host_connect.h translate.h
main.o: host_io.h client_io.h region.h encode.h
logging.o: logging.h
active.o: rfblib.h reflector.h logging.h
actions.o: rfblib.h reflector.h logging.h
host_connect.o: rfblib.h reflector.h logging.h async_io.h host_io.h
host_connect.o: translate.h client_io.h region.h encode.h host_connect.h
d3des.o: d3des.h
rfblib.o: rfblib.h d3des.h
async_io.o: async_io.h
host_io.o: rfblib.h reflector.h async_io.h logging.h translate.h client_io.h
host_io.o: region.h host_connect.h host_io.h encode.h
client_io.o: rfblib.h logging.h async_io.h reflector.h host_io.h translate.h
client_io.o: client_io.h region.h encode.h
encode.o: rfblib.h reflector.h async_io.h translate.h client_io.h region.h
encode.o: encode.h
region.o: rfblib.h region.h
translate.o: rfblib.h reflector.h async_io.h translate.h client_io.h region.h
control.o: rfblib.h async_io.h logging.h reflector.h host_connect.h host_io.h
control.o: translate.h client_io.h region.h
encode_tight.o: rfblib.h reflector.h async_io.h translate.h client_io.h
encode_tight.o: region.h encode.h
decode_hextile.o: rfblib.h reflector.h async_io.h logging.h host_io.h
decode_tight.o: rfblib.h reflector.h async_io.h logging.h host_io.h
fbs_files.o: rfblib.h reflector.h logging.h
region_more.o: rfblib.h region.h logging.h
