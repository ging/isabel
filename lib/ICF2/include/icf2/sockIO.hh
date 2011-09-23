/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// $Id: sockIO.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__sock_io_hh__
#define __icf2__sock_io_hh__

#ifdef WIN32
#include <icf2/inet_ntop.hh>
#include <ws2tcpip.h>
#include <icf2/tpipv6.h>
#include <icf2/wspiapi.h>
#endif

#include <icf2/io.hh>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#endif

#ifdef IRIX
# include <sys/endian.h>
#endif

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif  /* INADDR_NONE */

#define DEFAULT_MCAST_TTL 32

#ifdef __BUILD_FOR_WINXP
typedef unsigned short int sa_family_t;
#define IN6_IS_ADDR_MULTICAST(a)        ((*a).s6_addr[0] == 0xff)
LPSTR _Dll_ DecodeError(int ErrorCode);
#endif

class _Dll_ serviceAddr_t: public virtual item_t
{
public:
    static bool hasIPv6Support(void);

public:
    enum    serviceType  { DGRAM= SOCK_DGRAM, STREAM= SOCK_STREAM };

    serviceAddr_t(void) { type= 0; len= 0; mcastFlag= 0; };
    virtual ~serviceAddr_t(void);

    int type;
    int len;
    sa_family_t family;


    union {
        struct sockaddr     ssaddr;
        struct sockaddr_in  ssin;
        struct sockaddr_in6 ssin6;
#ifdef __BUILD_FOR_LINUX
        struct sockaddr_un  ssun;
#endif
    } saddr;

    int mcastFlag;


    virtual int operator ==(const serviceAddr_t &other) const {
        assert(false && "Peligro, estoy comparando serviceAddr_t!");
        return 0; // to shut lint off
    };

    virtual const char *className(void) const { return "serviceAddr_t"; };
};


class _Dll_ inetAddr_t: public serviceAddr_t
{
private:
    void __testMcastAddress();

    char *addrStr;

public:

    inetAddr_t(const char *_h, const char *_s, int= STREAM);
    inetAddr_t(sockaddr *sockaddr, int _l, int _t);
    inetAddr_t(const inetAddr_t &other);
    inetAddr_t(void);

    virtual ~inetAddr_t(void);

    const char *getStrValue(void) const { return addrStr; }
    int         getPort(void)     const;
	
    void  toString(char *addrStr, int addrStrLen) const;


    // Compare IP addresses
    int equalIPAddr(const inetAddr_t &other) const;

    // Compare inetAddr_t objects
    // inetAddr_t and IP address could differ in the port number!
    virtual int operator ==(const inetAddr_t &other) const;
    inetAddr_t &operator =(const inetAddr_t &other);
    bool operator <(const inetAddr_t &other) const;
    bool operator >(const inetAddr_t &other) const;


    virtual const char *className(void) const { return "inetAddr_t"; };
};

#ifdef __BUILD_FOR_LINUX
class unixAddr_t: public serviceAddr_t
{
public:
    unixAddr_t(const char *_p= NULL);
    virtual ~unixAddr_t(void);


    virtual const char *className(void) const { return "unixAddr_t"; };
};
#endif


class _Dll_ socketIO_t: public io_t
{
public:
    mutable int localPort;

    enum socketType_t {
          SERVER_SOCK=1,
          CLIENT_SOCK
    };

protected:
    serviceAddr_t   *serviceAddr;
    void init(serviceAddr_t &);

    int _bind(serviceAddr_t &);

    void __getLocalPort(serviceAddr_t &s);

public:

    socketIO_t(serviceAddr_t &);
    virtual ~socketIO_t(void);

    void setSockMaxBuf(void);

    virtual const char *className(void) const { return "socketIO_t"; };
};



class _Dll_ dgramSocket_t: public socketIO_t
{
public:
    inetAddr_t    *clientAddr;
    inetAddr_t    *temporaryClientAddr;

    dgramSocket_t(inetAddr_t &);
    dgramSocket_t(inetAddr_t &, inetAddr_t &);
    virtual ~dgramSocket_t(void);

    virtual int read(void *, int);
    virtual int write(const void *, int);
    virtual int writeTo(const inetAddr_t &, const void *, int);
    virtual int recvFrom(inetAddr_t &, void *, int);

    virtual int joinGroup(const inetAddr_t &, int _mcastTTL=DEFAULT_MCAST_TTL);
    virtual int leaveGroup(const inetAddr_t &);

#if !defined(__BUILD_FOR_DARWIN)
// Darwin does not support SSM
    //public SSM interface
    // join leave SSM (source Address, group Address)
    int joinSSM( const inetAddr_t &, const inetAddr_t &);
    int leaveSSM( const inetAddr_t &, const inetAddr_t &);
#endif

    virtual const char *className(void) const { return "dgramSocket_t"; };
};


class _Dll_ streamSocket_t: public socketIO_t
{
private:
    socketType_t type;

public:
    streamSocket_t(inetAddr_t &, socketType_t socketType=SERVER_SOCK);
#ifdef __BUILD_FOR_LINUX
    streamSocket_t(unixAddr_t &, socketType_t socketType=SERVER_SOCK);
#endif
#ifdef WIN32
    streamSocket_t(streamSocket_t &s);
#endif

    virtual ~streamSocket_t(void);

    virtual streamSocket_t *accept(void);

    int connect(serviceAddr_t &_a);
    int listen(serviceAddr_t &_a);

#ifdef WIN32
    virtual int read(void *, int);
    virtual int write(const void *, int);
#endif

    virtual const char *className(void) const { return "streamSocket_t"; };
    void getRemoteAddr(char *addr, int len);
};


#endif


