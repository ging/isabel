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
// $Id: sockIO.cc 20756 2010-07-05 09:57:09Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <icf2/general.h>

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include <icf2/notify.hh>
#include <icf2/sockIO.hh>

// Maximum String Length for IP address
#define __ICF__MAXHOSTNAMELEN 256

#define __ICF_STR_BUF_LEN 1024


void
PrintSockError(const char * context)
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
        char __icf_strerr[__ICF_STR_BUF_LEN];
        strerror_r(errno, __icf_strerr, __ICF_STR_BUF_LEN);
        NOTIFY("%s: error: %s\n", context, __icf_strerr);
#elif defined(WIN32)
        NOTIFY("%s: error: %d\n", context, WSAGetLastError());
#else
#error "Please, define and open a socket in your O.S."
#endif
}

bool
IPv6_SUPPORT(void)
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    int s= ::socket(AF_INET6, SOCK_DGRAM, 0);
#elif defined(WIN32)
    SOCKET s=socket(AF_INET6, SOCK_DGRAM, 0);
#else
#error "Please, define and open a socket in your O.S."
#endif

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    if (s > 0)
    {
        close(s);
        return true;
    }
#elif defined(WIN32)
    if (s != INVALID_SOCKET)
    {
        int i= closesocket(s);
        return true;
    }
#else
#error "Please, check if your socket was successfully open in your O.S."
#endif

    return false;
}

static bool HAS_IPv6_SUPPORT= IPv6_SUPPORT();

#ifdef WIN32
bool
winsock_init(void)
{
    WORD version;
    WSADATA data;
    version=MAKEWORD(1,1);
    ::WSAStartup(version,&data); // iniciamos Winsock
    HAS_IPv6_SUPPORT= false;//IPv6_SUPPORT();
    return true;
}

static bool WINSOCK_INIT = winsock_init();

LPSTR DecodeError(int ErrorCode)
{
    static char Message[1024];

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
                  FORMAT_MESSAGE_MAX_WIDTH_MASK, NULL, ErrorCode,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPSTR)Message, 1024, NULL);
    return Message;
}
#endif


serviceAddr_t::~serviceAddr_t(void)
{
    return;
}

bool
serviceAddr_t::hasIPv6Support(void)
{
    return HAS_IPv6_SUPPORT;
}

// ---------------------------------------------------------------------
// inetAddr_t
//
// genera una direccion IPv4 o IPv6
// dependiendo del soporte de la maquina
// ---------------------------------------------------------------------

inetAddr_t::inetAddr_t(const char *_h, const char *_s, int t)
{
    addrinfo hints, *res;
    int      gai_error;

#if 0
    if (   (  (!_h) || (strcmp(_h, "0")==0))
        && ( ((!_s) || (strcmp(_s, "0")==0)))) {
        NOTIFY("inetAddr_t::inetAddr_t:: at least one of the "
               "two parameters should be NON NULL, host or service, "
               "system halted!\n"
              );
        abort();
    }
#endif

    type = t;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_socktype = t;

    if ((!_h) || (strcmp(_h, "0")==0)) {
        // _h & _s shouldn't be NULL simultaneously for getaddrinfo
        if (HAS_IPv6_SUPPORT) {
            hints.ai_family = AF_INET6;
            _h="0::0"; // IN6_UNSPECIFIED ADDR
        } else {
            hints.ai_family = AF_INET;
            _h="0.0.0.0"; // IN_UNSPECIFIED ADDR
        }
    }

    if (strcmp(_h, "localhost") == 0) { // allows "localhost"
        // _h & _s shouldn't be NULL simultaneously for getaddrinfo
        if (HAS_IPv6_SUPPORT) {
            hints.ai_family = AF_INET6;
            _h="::1"; // IN6_UNSPECIFIED ADDR
        } else {
            hints.ai_family = AF_INET;
            _h="127.0.0.1"; // IN_UNSPECIFIED ADDR
        }
    }

    if ((gai_error = getaddrinfo(_h, _s, &hints, &res)))
    {
        NOTIFY("inetAddr_t::ineAddr_t: getaddrinfo error:: "
               "address failed. Host: [%s] Port: [%s] Error: [%s]\n",
               _h, _s, gai_strerror(gai_error)
              );
        abort();
    }

    memset(&saddr.ssaddr, 0, sizeof(saddr));
    memcpy(&saddr.ssaddr, res->ai_addr, res->ai_addrlen);

    len = res->ai_addrlen;
    family = res->ai_family;

    // assert(hints.ai_family==res->ai_family);

    switch (res->ai_family) {
    case AF_INET6:
        debugMsg(dbg_App_Normal, "inetAddr_t", "AF_INET6 port[%d]", getPort());
        addrStr = (char *)malloc(__ICF__MAXHOSTNAMELEN);
        memset(addrStr, 0, __ICF__MAXHOSTNAMELEN);
        if (inet_ntop(family, (unsigned char *)&saddr.ssin6.sin6_addr,
                      addrStr, __ICF__MAXHOSTNAMELEN ) == NULL)
        {
            NOTIFY("inetAddr_t::inetAddr_t: IPv6 inet_ntop error: "
                   "network to presentation error!\n"
                  );
            abort();
        }
        break;

    case AF_INET:
        debugMsg(dbg_App_Normal, "inetAddr_t", "AF_INET port[%d]", getPort());
        addrStr = (char *)malloc(__ICF__MAXHOSTNAMELEN);
        memset(addrStr, 0, __ICF__MAXHOSTNAMELEN);
        if (inet_ntop(family, (unsigned char *)&saddr.ssin.sin_addr,
                      addrStr, __ICF__MAXHOSTNAMELEN) == NULL)
        {
            NOTIFY("inetAddr_t::inetAddr_t: IPv4 inet_ntop error: "
                   "network to presentation error!\n"
                  );
            abort();
        }
        break;

    default:
        NOTIFY("inetAddr_t::inetAddr_t(char *, char *, int): "
               "Unknown family!! not AF_INET6, not AF_INET\n"
              );
        abort();
    }

   __testMcastAddress();

    freeaddrinfo(res);
}

inetAddr_t::inetAddr_t(sockaddr *sockaddr, int _l, int _t)
{
    if ( ! sockaddr)
    {
        NOTIFY("inetAddr_t::inetAddr_t: could not build an "
               "inetAddr_t object from a NULL sockaddr struct\n"
               "Are you sure you want to call to this constructor: \n"
               "\tinetAddr_t::inetAddr_t(sockaddr *sockaddr, int _l, int _t) "
               "?\n"
              );
        abort();
    }

    memcpy(&saddr.ssaddr, sockaddr, _l);
    len =_l;

    debugMsg(dbg_App_Paranoic, "inetAddr_t::inetAddr_t:",
             "Construyendo len=%d family=%d len(sockaddr_in6)=%d "
             "len(sockaddr_in)=%d family(AF_INET6)=%d family(AF_INET)=%d\n",
             len, sockaddr->sa_family, sizeof(struct sockaddr_in6),
             sizeof(struct sockaddr_in), AF_INET6, AF_INET
            );

// CHAPUZA! NO LO PUEDO QUITAR, CON LAS DIRECCIONES 0.0.0.0 family=0!
#if 1
    if (len == sizeof(struct sockaddr_in))
       family = AF_INET;

    else if ((len == sizeof(struct sockaddr_in6)) ||
             (len == sizeof(struct sockaddr_in6) + 4 /*chapuza RFC 2553*/)
            )
       family = AF_INET6;

    else
        family = sockaddr->sa_family;
#endif

    type =_t;

    switch (family) {
    case AF_INET:
        addrStr = (char *)malloc(__ICF__MAXHOSTNAMELEN);
        inet_ntop(family,
                  (unsigned char *)&saddr.ssin.sin_addr.s_addr,
                  addrStr,
                  __ICF__MAXHOSTNAMELEN
                 );
        break;

    case AF_INET6:
        addrStr = (char *)malloc(__ICF__MAXHOSTNAMELEN);
        inet_ntop(family,
                  (unsigned char *)saddr.ssin6.sin6_addr.s6_addr,
                  addrStr,
                  __ICF__MAXHOSTNAMELEN
                 );
        break;

    default:
        NOTIFY("inetAddr_t::inetAddr_t(sockaddr, int, int): "
               "Unknown address family [%d]! not AF_INET6, not AF_INET\n",
               family
              );
        abort();
    }
}

inetAddr_t::inetAddr_t(const inetAddr_t &other)
{
    type      = other.type;
    len       = other.len;
    family    = other.family;
    mcastFlag = other.mcastFlag;
    addrStr   = other.addrStr == NULL ? NULL : strdup(other.addrStr);

    memset(&saddr, 0, sizeof(saddr));
    memcpy(&saddr, &other.saddr, len);
}

inetAddr_t::inetAddr_t(void)
{
    type      = 0;
    memset(&saddr, 0, sizeof(saddr));
    family    = 0;
    len       = sizeof(saddr);
    addrStr   = NULL;
    mcastFlag = 0;
}

void
inetAddr_t::toString(char *addrStr, int addrStrLen) const
{
    const char *res;

    sockaddr_in  * IP4 = NULL;
    sockaddr_in6 * IP6 = NULL;

    switch (family)
    {
    case AF_INET:
        {
            IP4 = (sockaddr_in *)(&saddr.ssin);
            res= inet_ntop(IP4->sin_family,(unsigned char *)&(IP4->sin_addr),addrStr,addrStrLen);
            break;
        }
    case AF_INET6:
        {
            IP6 = (sockaddr_in6 *)(&saddr.ssin6);
            res= inet_ntop(IP6->sin6_family,(unsigned char *)&(IP6->sin6_addr),addrStr,addrStrLen);
            break;
        }
    default:
        NOTIFY("inetAddr_t::toString: "
               "Unknown address family [%d]! not AF_INET6, not AF_INET\n",
               family
              );
        abort();
    }

    if (res == NULL)
    {
         NOTIFY("inetAddr_t::toString: IPv6 inet_ntop error: "
                "network to presentation error! %s\n",
                strerror(errno)
               );
         abort();
    }
}

int
inetAddr_t::getPort(void) const
{
    int port = 0;
    switch (family)
    {
    case AF_INET:
        port = ntohs(saddr.ssin.sin_port);
        break;
    case AF_INET6:
        port = ntohs(saddr.ssin6.sin6_port);
        break;
    default:
        NOTIFY("inetAddr_t::getPort(void): "
               "Unknown address family [%d]! not AF_INET6, not AF_INET\n",
               family);
    }
    return port;
}

void
inetAddr_t::__testMcastAddress(void)
{
    switch (family)
    {
    case AF_INET6:
        if (IN6_IS_ADDR_MULTICAST(&saddr.ssin6.sin6_addr))
        {
            if(type!= DGRAM) {
                NOTIFY("inetAddr_t::inetAddr_t:: IPv6 multicast "
                       "addresses only using DGRAM_SOCK, system halted!\n"
                      );
                abort();
            }
            mcastFlag= 1;
        }
        break;
    case AF_INET:
        //
        // test for IP class D
        // ntohl(saddr.ssin.sin_addr.s_addr) & 0xF0000000) == 0xE0000000)

        if(IN_MULTICAST(ntohl(saddr.ssin.sin_addr.s_addr))) {
            if(type!= DGRAM) {
                NOTIFY("inetAddr_t::inetAddr_t:: IPv4 multicast "
                       "addresses only using DGRAM_SOCK, system halted!\n"
                      );
                abort();
            }
            mcastFlag= 1;
        }
        break;
    default:
        NOTIFY("inetAddr_t::__testMcastAddress:: "
               "Unknown family!! not AF_INET6, not AF_INET\n"
              );
    }
}



inetAddr_t::~inetAddr_t(void)
{
    if (addrStr != NULL)
    {
        free(addrStr);
        addrStr= NULL;
    }
}


int
inetAddr_t::operator ==(const inetAddr_t &other) const
{
    if (family != other.family) return 0;

    switch (family) {
    case  AF_INET:
        // No comparo las estructuras porque solo se usan 8 de los 16 bytes
        // en los 8 restantes no se que hay!!!
        return (saddr.ssin.sin_family == other.saddr.ssin.sin_family) &&
               (saddr.ssin.sin_port == other.saddr.ssin.sin_port) &&
               (memcmp(&saddr.ssin.sin_addr.s_addr,
                       &other.saddr.ssin.sin_addr.s_addr,
                       sizeof(saddr.ssin.sin_addr.s_addr)
                      ) == 0
               );

    case AF_INET6:
        return memcmp(&saddr.ssin6,
                      &other.saddr.ssin6,
                      sizeof(saddr.ssin6)
                     ) == 0;

    default:
        NOTIFY("inetAddr_t::==:: Unknown family (%d)!!\n", family);
        abort();
    }
    return 0;
}

bool 
inetAddr_t::operator <(const inetAddr_t &other) const
{
    return (memcmp(&saddr.ssaddr, &other.saddr.ssaddr, sizeof(saddr.ssaddr)));
}

bool 
inetAddr_t::operator >(const inetAddr_t &other) const 
{
    return (memcmp(&saddr.ssaddr, &other.saddr.ssaddr, sizeof(saddr.ssaddr)));
}

int
inetAddr_t::equalIPAddr(const inetAddr_t &other) const
{
    if (family != other.family) return 0;

    // Ojo! no puedo comparar las estructuras xq cuando leo
    // de la red un pkt el origen llev impreso su localPort
    // q puede ser diferente en varias conex (caso irouter)
    switch (family) {
    case  AF_INET:
        // No comparo las estructuras porque solo se usan 8 de los 16 bytes
        // en los 8 restantes no se que hay!!!
        return memcmp(&saddr.ssin.sin_addr.s_addr,
                      &other.saddr.ssin.sin_addr.s_addr,
                      sizeof(saddr.ssin.sin_addr.s_addr)
                     ) == 0;
    case AF_INET6:
        return memcmp(&saddr.ssin6.sin6_addr,
                      &other.saddr.ssin6.sin6_addr,
                      sizeof(saddr.ssin6.sin6_addr)
                     ) == 0;
    default:
        NOTIFY("inetAddr_t::equalIPAddr: Unknown family (%d)!!\n", family);
        abort();
    }
    return 0;
}

inetAddr_t &
inetAddr_t::operator=(const inetAddr_t &other)
{
    type      = other.type;
    len       = other.len;
    family    = other.family;
    mcastFlag = other.mcastFlag;
    if (addrStr)
    {
        free(addrStr);
    }
    addrStr= other.addrStr == NULL ? NULL : strdup(other.addrStr);

    memset(&saddr, 0, sizeof(saddr));
    memcpy(&saddr, &other.saddr, len);

    return *this;
}


#ifdef __BUILD_FOR_LINUX
unixAddr_t::unixAddr_t(const char *_p)
{
    struct sockaddr_un  &ssun= saddr.ssun;

    len= sizeof(ssun);
    type= STREAM;

    memset(&ssun, 0, sizeof(ssun));
    ssun.sun_family= AF_UNIX;

    strcpy(ssun.sun_path, _p);
}

unixAddr_t::~unixAddr_t(void)
{
}
#endif


socketIO_t::socketIO_t(serviceAddr_t &_a)
: io_t(-1)
{
    init(_a);
}

socketIO_t::~socketIO_t(void)
{
    delete serviceAddr;
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    close(__prot__handle->sysHandle);
#elif defined(WIN32)
    closesocket(__prot__handle->sysHandle);
#else
#error "Please, check if socket must be closed or it is done in ~io_t"
#endif
}

void
socketIO_t::init(serviceAddr_t &_a)
{
    signed int uno= 1;
    serviceAddr= new serviceAddr_t(_a);
    __prot__handle->sysHandle= ::socket(_a.saddr.ssaddr.sa_family, _a.type, 0);

    ::setsockopt(__prot__handle->sysHandle,
                 SOL_SOCKET,
                 SO_REUSEADDR,
                 (char *)&uno,
                 sizeof(uno)
                );
    __getLocalPort(_a);
}

void
socketIO_t::__getLocalPort(serviceAddr_t &_a)
{
    switch (_a.saddr.ssaddr.sa_family)
    {
    case AF_INET:
        struct sockaddr_in sin;
        int sin_len;
        memset((void *)&sin, 0, sizeof(sin));
        sin_len= sizeof(struct sockaddr_in);
        ::getsockname(__prot__handle->sysHandle,
                      (struct sockaddr *)&sin,
                      (socklen_t *)&sin_len
                     );
        if (sin.sin_port)
        {
            _a.saddr.ssin.sin_port = sin.sin_port;
            serviceAddr->saddr.ssin.sin_port = sin.sin_port;
        }
        localPort= ntohs(_a.saddr.ssin.sin_port);
        break;

    case AF_INET6:
        struct sockaddr_in6 sin6;
        int sin6_len;
        memset((void *)&sin6, 0, sizeof(sin6));
        sin6_len= sizeof(struct sockaddr_in6);
        ::getsockname(__prot__handle->sysHandle,
                      (struct sockaddr *)&sin6,
                      (socklen_t *)&sin6_len
                     );
        if (sin6.sin6_port)
        {
            _a.saddr.ssin.sin_port = sin6.sin6_port; 
            serviceAddr->saddr.ssin.sin_port = sin6.sin6_port; 
        }
        localPort= ntohs(_a.saddr.ssin.sin_port);
        break;

#ifdef __BUILD_FOR_LINUX
    case AF_UNIX:
        break;
#endif

    default:
        NOTIFY("socketIO_t::__getLocalPort: Problem, unknown family [%d]\n",
               _a.saddr.ssaddr.sa_family
              );
        abort();
    }
}

int
socketIO_t::_bind(serviceAddr_t &_a)
{
    int res = ::bind(__prot__handle->sysHandle, &_a.saddr.ssaddr, _a.len);

    if (res <0)
    {
        PrintSockError("socketIO_t::_bind");
    }
    else
    {
        __getLocalPort(_a);
    }

    return res;
}


void
socketIO_t::setSockMaxBuf(void)
{
    int good_res;
    int res_in;
    int res_out;
#ifdef WIN32
    int res_len= sizeof(int);
#else
    socklen_t res_len= sizeof(int);
#endif

    getsockopt(__prot__handle->sysHandle,
               SOL_SOCKET,
               SO_SNDBUF,
               (char *)&good_res,
               &res_len
              );
    res_in= res_out= good_res;

    while ((res_in == res_out) && (res_in <= 128 KBYTES))
    {
        res_in+= 4 KBYTES;
        res_len= sizeof(int);
        setsockopt(__prot__handle->sysHandle,
                   SOL_SOCKET,
                   SO_SNDBUF,
                   (char *)&res_in,
                   res_len
                  );
        getsockopt(__prot__handle->sysHandle,
                   SOL_SOCKET, SO_SNDBUF,
                   (char *)&res_out,
                   &res_len
                  );

        if (res_out == res_in)
        {
            good_res= res_in;
        }
    }

    res_len= sizeof(int);
    setsockopt(__prot__handle->sysHandle,
               SOL_SOCKET, SO_SNDBUF,
               (char *)&good_res,
               res_len
              );
    getsockopt(__prot__handle->sysHandle,
               SOL_SOCKET,
               SO_RCVBUF,
               (char *)&good_res,
               &res_len
              );
    res_in= res_out= good_res;

    while((res_in == res_out) && (res_in<= 128 KBYTES))
    {
        res_in+= 4 KBYTES;
        res_len= sizeof(int);
        setsockopt(__prot__handle->sysHandle,
                   SOL_SOCKET,
                   SO_RCVBUF,
                   (char *)&res_in,
                   res_len
                  );
        getsockopt(__prot__handle->sysHandle,
                   SOL_SOCKET,
                   SO_RCVBUF,
                   (char *)&res_out,
                   &res_len
                  );

        if(res_out== res_in)
            good_res= res_in;
    }

    res_len= sizeof(int);
    setsockopt(__prot__handle->sysHandle,
               SOL_SOCKET,
               SO_RCVBUF,
               (char *)&good_res,
               res_len
              );
}


//
// dgramSocket_t -- datagram service
//
//  [ can have mcast service ]
//
dgramSocket_t::dgramSocket_t(inetAddr_t &_a)
: socketIO_t(_a)
{
    if (_bind(_a) < 0) // Habria que cambiarlo!
    {
        NOTIFY("dgramSocket_t:: Error binding server socket, system halted!\n");
        abort();
    }

    if(_a.type!= serviceAddr_t::DGRAM)
    {
        NOTIFY("dgramSocket_t:: Not a DGRAM address, system halted!\n");
        abort();
    }

    clientAddr= temporaryClientAddr= NULL;
}

dgramSocket_t::dgramSocket_t(inetAddr_t &_a, inetAddr_t &_a2): socketIO_t(_a)
{
    if (_bind(_a) < 0) // Habria que cambiarlo!
    {
        NOTIFY("dgramSocket_t:: Error binding server socket: system halted!\n");
        abort();
    }

    if (_a.type != serviceAddr_t::DGRAM)
    {
        NOTIFY("dgramSocket_t:: Not a DGRAM address, system halted!\n");
        abort();
    }

    if (_a2.type != serviceAddr_t::DGRAM)
    {
        NOTIFY("dgramSocket_t:: Not a DGRAM address, system halted!\n");
        abort();
    }

    if (_a.family != _a2.family)
    {
        char fourZeroes[4];
        memset(fourZeroes, 0, 4);

        if (memcmp(&_a.saddr.ssaddr.sa_data[2], fourZeroes, 4) == 0)
        {
            NOTIFY("dgramSocket_t:: warning!! both address "
                   "belong to different family: %s and %s\n",
                    _a.family  == AF_INET6 ? "AF_INET6" : "AF_INET",
                    _a2.family == AF_INET6 ? "AF_INET6" : "AF_INET"
                  );
        }
        else
        {
            NOTIFY("dgramSocket_t:: warning!! both address should "
                   "belong to the same family, default AF_INET\n"
                  );
            abort();
        }
    }
    clientAddr= new inetAddr_t(_a2);
    temporaryClientAddr= NULL;
}

dgramSocket_t::~dgramSocket_t(void)
{
    if (clientAddr)
    {
        delete clientAddr;
    }
    if (temporaryClientAddr)
    {
        delete temporaryClientAddr;
    }
}

int
dgramSocket_t::read(void *b, int n)
{
    if (clientAddr)
    {
        inetAddr_t scratch;
        return recvFrom(scratch, b, n);
    }
    else
    {
        if ( ! temporaryClientAddr)
        {
            temporaryClientAddr= new inetAddr_t;
        }

        return recvFrom(*temporaryClientAddr, b, n);
    }
}

int
dgramSocket_t::write(const void *b, int n)
{
    if (clientAddr)
    {
        return writeTo(*clientAddr, b, n);
    }
    else if (temporaryClientAddr)
    {
        return writeTo(*temporaryClientAddr, b, n);
    }

    return 0;
}

int
dgramSocket_t::recvFrom(inetAddr_t &_a, void *b, int n)
{
    int result= ::recvfrom(__prot__handle->sysHandle,
                           (char *)b,
                           n,
                           0,
                           (sockaddr *)&_a.saddr.ssin6,
                           (socklen_t*)&_a.len
                          );

    //if (result < 0)
    //{
    //    PrintSockError("dgramSocket_t::recvFrom");
    //    return result;
    //}

    _a = inetAddr_t(&_a.saddr.ssaddr, _a.len, serviceAddr_t::DGRAM);

    return result;
}

int
dgramSocket_t::writeTo(const inetAddr_t &_a, const void *b, int n)
{
    int result = ::sendto(__prot__handle->sysHandle,
                          (char*)b,
                          n,
                          0,
                          &_a.saddr.ssaddr,
                          _a.len
                         );

    if (result < 0)
    {
        NOTIFY("dgramSocket_t::writeTo: ERROR on socket %d (%s)\n",
               __prot__handle->sysHandle,
               _a.getStrValue()
              );
        PrintSockError("dgramSocket_t::writeTo");
    }

    return result;
}

int
dgramSocket_t::joinGroup(const inetAddr_t &_a, int _mcastTTL)
{
    if ( ! _a.mcastFlag)
    {
        NOTIFY("dgramSocket_t::joinGroup: non multicast address\n");
    }

    int r1=-1;
    int r2=-1;
    int r3=-1;
    switch (_a.family)
    {
    case AF_INET: {
        i8                  loopBack= 0;
        //i8                  mcastTTL= DEFAULT_MCAST_TTL;
        i8                  mcastTTL= (i8)_mcastTTL;
        struct ip_mreq      mreq;

        mreq.imr_multiaddr.s_addr= _a.saddr.ssin.sin_addr.s_addr;
        mreq.imr_interface.s_addr= INADDR_ANY;

        r1= setsockopt(__prot__handle->sysHandle,
                       IPPROTO_IP,
                       IP_MULTICAST_LOOP,
                       (char *)&loopBack,
                       sizeof(loopBack)
                      );
        if (r1 < 0)
        {
            PrintSockError("dgramSocket_t::writeTo");
        }

        r2= setsockopt(__prot__handle->sysHandle,
                       IPPROTO_IP,
                       IP_MULTICAST_TTL,
                       (char *)&mcastTTL,
                       sizeof(mcastTTL)
                      );
        if (r2 < 0)
        {
            PrintSockError("ICF2 Warning:: joinGroup:: IP_MULTICAST_TTL");
        }

        r3= setsockopt(__prot__handle->sysHandle,
                       IPPROTO_IP,
                       IP_ADD_MEMBERSHIP,
                       (char*)&mreq,
                       sizeof(mreq)
                      );
        if (r3 < 0)
        {
            PrintSockError("ICF2 Warning:: joinGroup:: IP_ADD_MEMBERSHIP");
        }

    } break;

    case AF_INET6: {
        int                 loopBack= 0;
        int                 mcastTTL= _mcastTTL;
        struct ipv6_mreq    mreq6;

        memcpy(&mreq6.ipv6mr_multiaddr,
               &_a.saddr.ssin6.sin6_addr,
               sizeof(struct in6_addr)
              );

        mreq6.ipv6mr_interface= 0; // cualquier interfaz

        r1= setsockopt(__prot__handle->sysHandle,
                       IPPROTO_IPV6,
                       IPV6_MULTICAST_LOOP,
                       (char *)&loopBack,
                       sizeof(loopBack)
                      );
        if (r1 < 0)
        {
            PrintSockError("ICF2 Warning:: joinGroup:: IPV6_MULTICAST_LOOP:: ");
        }

        r2= setsockopt(__prot__handle->sysHandle,
                       IPPROTO_IPV6,
                       IPV6_MULTICAST_HOPS,
                       (char *)&mcastTTL,
                       sizeof(mcastTTL)
                      );
        if (r2 < 0)
        {
            PrintSockError("ICF2 Warning:: joinGroup:: IPV6_MULTICAST_HOPS::  ");
        }

        r3= setsockopt(__prot__handle->sysHandle,
                       IPPROTO_IPV6,
                       IPV6_JOIN_GROUP,
                       (char *) &mreq6,
                       sizeof(mreq6)
                      );
        if (r3 < 0)
        {
            PrintSockError("ICF2 Warning:: joinGroup:: IPV6_JOIN_GROUP:: ");
        }
    } break;

    default:
        NOTIFY("dgramSocket_t::joinGroup:: could not joinGroup with "
               "address not belonging to AF_INET or AF_INET6 families\n"
              );
        abort();
    }

    int r= (r1>= 0) && (r2>= 0) && (r3>= 0);
    if ( ! r)
    {
        NOTIFY("dgramSocket_t::joinGroup: it seems that this machine "
               "doesn't support IP multicast :-(\n"
              );
    }

    return r;
}

int
dgramSocket_t::leaveGroup(const inetAddr_t &_a)
{
    if ( ! _a.mcastFlag)
    {
        NOTIFY("dgramSocket_t::leaveGroup: non multicast address\n");
    }

    int ret= -1;

    switch (_a.family)
    {
    case AF_INET:
        {
            struct ip_mreq      mreq;
            mreq.imr_multiaddr.s_addr= _a.saddr.ssin.sin_addr.s_addr;
            mreq.imr_interface.s_addr= INADDR_ANY;

            ret= setsockopt(__prot__handle->sysHandle,
                            IPPROTO_IP,
                            IP_DROP_MEMBERSHIP,
                            (char  *)&mreq,
                            sizeof(mreq)
                           );
            if (ret < 0)
            {
                perror("ICF2 Warning:: leaveGroup:: IP_DROP_MEMBERSHIP:: ");
            }
        }
        break;
    case AF_INET6:
        {
            struct ipv6_mreq    mreq6;

            memcpy(&mreq6.ipv6mr_multiaddr,
                   &_a.saddr.ssin6.sin6_addr,
                   sizeof(struct in6_addr)
                  );

            mreq6.ipv6mr_interface= 0; // cualquier interfaz

            ret= setsockopt(__prot__handle->sysHandle,
                            IPPROTO_IPV6,
                            IPV6_LEAVE_GROUP,
                            (char *) &mreq6,
                            sizeof(mreq6)
                           );
            if (ret < 0)
            {
                perror("ICF2 Warning:: leaveGroup:: IPV6_LEAVE_GROUP:: ");
            }
        }
        break;
    default:
        NOTIFY("leaveGroup:: Could not joinGroup with address "
               "different to AF_INET or AF_INET6 families\n"
              );
    }

    return ret;
}


#ifdef __BUILD_FOR_LINUX
// Joining and leaving Source Specific Multicast groups
int
dgramSocket_t::joinSSM(const inetAddr_t &_src, const inetAddr_t &_grp)
{
    struct group_source_req gsr;
    int level;
    int family;
    int addrlen;

    if (_src.family != _grp.family)
    {
        NOTIFY("dgramSocket_t::joinSSM Source and group addresses must belong to the same family\n");
        return 1;
    }

    switch(_src.family)
    {
    case AF_INET:
        family = AF_INET;
        level = IPPROTO_IP;
        addrlen = sizeof(struct sockaddr_in);
        break;
    case AF_INET6:
        family = AF_INET6;
        level = IPPROTO_IPV6;
        addrlen = sizeof(struct sockaddr_in6);

        break;
    default:
         NOTIFY("dgramSocket_t::joinSSM Unknown address family\n");
         abort();
    }

    memset(&gsr, 0, sizeof(gsr));
    memcpy(&gsr.gsr_source, &_src.saddr.ssaddr, addrlen);
    memcpy(&gsr.gsr_group, &_grp.saddr.ssaddr, addrlen);
    gsr.gsr_interface = INADDR_ANY;

    int ret= setsockopt(__prot__handle->sysHandle,
                        level,
                        MCAST_JOIN_SOURCE_GROUP,
                        (char *)&gsr,
                        sizeof(gsr)
                       );

    if (ret != 0)
    {
        NOTIFY("dgramSocket_t::joinSSM Unable to setsockopt \n");
        return 1;
    }

    return 0;
}


int
dgramSocket_t::leaveSSM(const inetAddr_t &_src, const inetAddr_t &_grp)
{
    struct group_source_req gsr;
    int level;
    int addrlen;
    int family;

    if (_src.family != _grp.family){
        NOTIFY("dgramSocket_t::leaveSSM Source and group addresses must belong to the same family\n");
        return 1;
    }

    switch(_src.family)
    {
    case AF_INET:
         family = AF_INET;
         level = IPPROTO_IP;
         addrlen = sizeof(struct sockaddr_in);
         break;
    case AF_INET6:
         family = AF_INET6;
         level = IPPROTO_IPV6;
         addrlen = sizeof(struct sockaddr_in6);
         break;
    default:
         NOTIFY("dgramSocket_t::joinSSM Unknown address family\n");
         return 1;
    }

    memset(&gsr, 0, sizeof(gsr));
    memcpy(&gsr.gsr_source, &_src.saddr.ssaddr, addrlen);
    memcpy(&gsr.gsr_group, &_grp.saddr.ssaddr, addrlen);
    gsr.gsr_interface = INADDR_ANY;

    int ret= setsockopt(__prot__handle->sysHandle,
                        level,
                        MCAST_LEAVE_SOURCE_GROUP,
                        (char *)&gsr,
                        sizeof(gsr)
                       );

    if (ret != 0)
    {
        NOTIFY("dgramSocket_t::leaveSSM Unable to setsockopt \n");
        return 1;
    }

    return 0;
}
#else
#ifdef __BUILD_FOR_WINXP
// Joining and leaving Source Specific Multicast groups
int
dgramSocket_t::joinSSM(const inetAddr_t &_src, const inetAddr_t &_grp)
{
    struct ip_mreq_source imr;

    if (_src.family != _grp.family)
    {
        NOTIFY("dgramSocket_t::joinSSM Source and group addresses "
               "must belong to the same family\n"
              );
        abort();
    }

    int ret= -1;

    switch(_src.family)
    {
    case AF_INET:
        {
            imr.imr_interface.s_addr = INADDR_ANY;
            imr.imr_sourceaddr.s_addr = _src.saddr.ssin.sin_addr.s_addr;
            imr.imr_multiaddr.s_addr = _grp.saddr.ssin.sin_addr.s_addr;

            ret= setsockopt(__prot__handle->sysHandle,
                            IPPROTO_IP,
                            IP_ADD_SOURCE_MEMBERSHIP,
                            (char *)&imr,
                            sizeof(imr)
                           );

            if (ret != 0)
            {
                int error = WSAGetLastError();
                NOTIFY("dgramSocket_t::joinSSM "
                       "Unable to setsockopt, wsaError: %d",
                       error
                      );
                return 1;
            }

            return 0;
        }
    case AF_INET6:
        NOTIFY("dgramSocket_t::joinSSM is not currently supported for Ipv6\n");
        return 1;
    default:
        NOTIFY("dgramSocket_t::joinSSM Could not joinGroup with address "
               "different to AF_INET or AF_INET6 families\n"
              );
        abort();
    }
    return 0; // to shut lint off
}

int
dgramSocket_t::leaveSSM(const inetAddr_t &_src, const inetAddr_t &_grp)
{
    struct ip_mreq_source imr;

    if (_src.family != _grp.family)
    {
        NOTIFY("dgramSocket_t::leaveSSM "
               "Source and group addresses must belong to the same family\n"
              );
        return 1;
    }

    switch(_src.family)
    {
    case AF_INET:
        {
            imr.imr_interface.s_addr = INADDR_ANY;
            imr.imr_sourceaddr.s_addr = _src.saddr.ssin.sin_addr.s_addr;
            imr.imr_multiaddr.s_addr = _grp.saddr.ssin.sin_addr.s_addr;

            int ret= setsockopt(__prot__handle->sysHandle,
                                IPPROTO_IP,
                                IP_DROP_SOURCE_MEMBERSHIP,
                                (char *)&imr,
                                sizeof(imr)
                               );
            if (ret != 0)
            {
                NOTIFY("dgramSocket_t::leaveSSM: "
                       "Unable to setsockopt, wsaError: %d",
                       WSAGetLastError()
                      );
                return 1;
            }

            return 0;
        }
    case AF_INET6:
        NOTIFY("dgramSocket_t::leaveSSM is not currently supported for Ipv6\n");
        return 1;
    default:
        NOTIFY("dgramSocket_t::leaveSSM Could not joinGroup with address "
               "different to AF_INET or AF_INET6 families\n"
              );
        abort();
    }

    return 0; // to shut lint off
}
#endif
#endif

//
// streamSocket_t -- stream service
//
//   [ to be documented]
//
streamSocket_t::streamSocket_t(inetAddr_t &_a, socketType_t socketType)
: socketIO_t(_a), type(socketType)
{
    if (_a.type != serviceAddr_t::STREAM)
    {
        NOTIFY("streamSocket_t:: not a STREAM address!, system halted!\n");
        abort();
    }
}

#ifdef __BUILD_FOR_LINUX
streamSocket_t::streamSocket_t(unixAddr_t &_a, socketType_t socketType)
: socketIO_t(_a), type(socketType)
{
}
#endif

#ifdef WIN32
streamSocket_t::streamSocket_t(streamSocket_t &s)
: socketIO_t(*s.serviceAddr)
{
    this->__prot__handle->sysHandle=s.__prot__handle->sysHandle;
    localPort=s.localPort ;
    type=s.type ;
}
#endif

streamSocket_t::~streamSocket_t(void)
{
}


int
streamSocket_t::listen(serviceAddr_t &_a)
{
    assert((type == SERVER_SOCK) && "Binding a client socket????");

    debugMsg(dbg_K_Verbose, "listening", "active, trying...");

    int res = _bind(_a);

    if (res < 0)
    {
        PrintSockError("streamSocket_t::listen");
    }
    else
    {
         ::listen(__prot__handle->sysHandle, 128);
    }

    return res;
}

int
streamSocket_t::connect(serviceAddr_t &_a2)
{
    assert((type == CLIENT_SOCK) && "Connecting a server socket????");

    debugMsg(dbg_K_Verbose, "connect", "active, trying...");

    int res= ::connect(__prot__handle->sysHandle, &_a2.saddr.ssaddr, _a2.len);

    if (res < 0)
    {
        PrintSockError("streamSocket_t::connect");
    }
    else
    {
        debugMsg(dbg_K_Verbose, "connect", "active, connected...");
    }

    return res;
}

streamSocket_t *
streamSocket_t::accept(void)
{
    streamSocket_t  *retVal;

    retVal= new streamSocket_t(*this);  // dup me
    retVal->serviceAddr= new serviceAddr_t(*this->serviceAddr);

    retVal->__prot__handle=
        new __prot__handle_t(::accept(__prot__handle->sysHandle,
                                      &retVal->serviceAddr->saddr.ssaddr,
                                      (socklen_t *)&retVal->serviceAddr->len
                                     )
                            );

    return retVal;
}

void
streamSocket_t::getRemoteAddr(char *addr, int len)
{
    inet_ntop(serviceAddr->family,
              (unsigned char *)&(serviceAddr->saddr.ssin.sin_addr),
              addr,
              len
             );
}

#ifdef WIN32
int
streamSocket_t::read(void *buf, int n)
{
    return ::recv(__prot__handle->sysHandle, (char*)buf, n, 0);
}

int
streamSocket_t::write(const void *buf, int n)
{
    return ::send(__prot__handle->sysHandle, (char*)buf, n, 0);
}
#endif

