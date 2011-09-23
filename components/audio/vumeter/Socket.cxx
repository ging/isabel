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

#include "Socket.h"
#include <errno.h>

#ifdef WIN32

#else

extern int errno;

#endif

static int SODEBUG = 0;

Address::Address(int protocol) {
  this->protocol = protocol;
  switch (protocol) {
  case PF_INET: {
    struct sockaddr_in addr4;
    addr4.sin_addr.s_addr = INADDR_ANY;
    addr4.sin_port = 0;
    memcpy(&ss, &addr4, sizeof(struct sockaddr_in));
    break;
  }
  case PF_INET6: {
    struct sockaddr_in6 addr6;
    addr6.sin6_addr = in6addr_any;
    addr6.sin6_port = 0;
    memcpy(&ss, &addr6, sizeof(struct sockaddr_in6));
    break;
  }
  }
}

Address::Address(Address *address) {
  protocol = address->protocol;
  ss = address->ss;
}

Address::Address(struct sockaddr *address, int protocol) {
  this->protocol = protocol;
  switch (protocol) {
  case PF_INET: {
    struct sockaddr_in *addr4 = reinterpret_cast<struct sockaddr_in*>(address);
    memcpy(&ss, addr4, sizeof(struct sockaddr_in));
    break;
  }
  case PF_INET6: {
    struct sockaddr_in6 *addr6 = reinterpret_cast<struct sockaddr_in6*>(address);
    memcpy(&ss, addr6, sizeof(struct sockaddr_in6));
    break;
  }
  }
}

Address::~Address() {
}

int Address::Resolve(const char *hostname, int port) {
  char thehostname[1024] = "";
  int server = 0;

  if (hostname) {
    if (hostname[0] == '[') {
      strncpy(thehostname, hostname+1, strlen(hostname)-2);
      thehostname[strlen(hostname)-2] = '\0';
    } else {
      strcpy(thehostname, hostname);
    }
  } else {
    server = 1;
  }
#ifdef WIN32
  struct addrinfo hints, *res;
  char portS[1024];
  struct sockaddr_in addr4; 
  struct sockaddr_in6 addr6;

  if ( server ) {
    // Es el truco para pedir el ANY, y hacer un servidor en un puerto
    // Usamos IPv4 o IPv6 en funcion de lo que se especificase al crear la address
    switch ( protocol ) {
    case PF_INET: {
      addr4.sin_family = AF_INET;
      addr4.sin_addr.s_addr = INADDR_ANY;
      addr4.sin_port = htons(port);
      memcpy(&ss, &addr4, sizeof(struct sockaddr_in));
      break;
    } 
    case PF_INET6: {
      addr6.sin6_family = AF_INET6;
      addr6.sin6_addr = in6addr_any;
      addr6.sin6_port = htons(port);
      memcpy(&ss, &addr6, sizeof(struct sockaddr_in6));
      break;
    }    
    }
    return 1;
  } 
  // Empleamos el getaddrinfo como ultimo recurso, para resolver el nombre en LO-QUE-SEA
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = PF_UNSPEC;
    hints.ai_protocol = 0;
    sprintf(portS, "%hd", port);
    int result;

    if ( (result = getaddrinfo(thehostname, portS, &hints, &res)) < 0) {
      // Ha fallado todo
      printf("Error en getaddrinfo: %s\n", gai_strerror(result));
      return -1;
    } else {
	  if (res) {
//      protocol = res->ai_protocol;
        memcpy(&ss, res->ai_addr, res->ai_addrlen);
        freeaddrinfo(res);
		return 1;
	  }
  struct hostent *host;
	long saddr;
if ( (host = gethostbyname(thehostname)) ) {
    if (SODEBUG) printf("Resolvemos en IPv4\n");
    // Era un nombre, que hemos resuelto en IPv4
    saddr = *reinterpret_cast<long*>(host->h_addr);
    protocol = PF_INET;
    addr4.sin_family = protocol;
    addr4.sin_port = htons(port);
    addr4.sin_addr.s_addr = saddr;
    memcpy(&ss, &addr4, sizeof(struct sockaddr_in));
  }
    }
#else

  struct sockaddr_in addr4; 
  struct sockaddr_in6 addr6;
  struct hostent *host;
  long s_addr;
  struct addrinfo hints, *res;
  char portS[1024];
  
  if ( server ) {
    // Es el truco para pedir el ANY, y hacer un servidor en un puerto
    // Usamos IPv4 o IPv6 en funcion de lo que se especificase al crear la address
    switch ( protocol ) {
    case PF_INET: {
      addr4.sin_family = AF_INET;
      addr4.sin_addr.s_addr = INADDR_ANY;
      addr4.sin_port = htons(port);
      memcpy(&ss, &addr4, sizeof(struct sockaddr_in));
      break;
    } 
    case PF_INET6: {
      addr6.sin6_family = AF_INET6;
      addr6.sin6_addr = in6addr_any;
      addr6.sin6_port = htons(port);
      memcpy(&ss, &addr6, sizeof(struct sockaddr_in6));
      break;
    }    
	}
    return 1;
  } 
  
  if ( inet_pton(PF_INET, thehostname, &addr4.sin_addr) ) {
    if (SODEBUG) printf("Direccion numerica en IPv4\n");
    // Es numerica, en IPv4
    protocol = PF_INET;
    addr4.sin_family = protocol;
    addr4.sin_port = htons(port);
    memcpy(&ss, &addr4, sizeof(struct sockaddr_in));
  } else if ( inet_pton(PF_INET6, thehostname, &addr6.sin6_addr) ) {
    if (SODEBUG) printf("Direccion numerica en IPv6\n");
    // Es numerica, en IPv6
    protocol = PF_INET6;
    addr6.sin6_family = protocol;
    addr6.sin6_port = htons(port);
    memcpy(&ss, &addr6, sizeof(struct sockaddr_in6));
  } else if ( (host = gethostbyname(thehostname)) ) {
    if (SODEBUG) printf("Resolvemos en IPv4\n");
    // Era un nombre, que hemos resuelto en IPv4
    s_addr = *reinterpret_cast<long*>(host->h_addr);
    protocol = PF_INET;
    addr4.sin_family = protocol;
    addr4.sin_port = htons(port);
    addr4.sin_addr.s_addr = s_addr;
    memcpy(&ss, &addr4, sizeof(struct sockaddr_in));
  } else if ( (host = gethostbyname2(thehostname, AF_INET6)) ) {
    if (SODEBUG) printf("Resolvemos en IPv6\n");
    // Era un nombre, que hemos resuelto en IPv6
    protocol = PF_INET6;
    addr6.sin6_family = protocol;
    addr6.sin6_port = htons(port);
    memcpy(addr6.sin6_addr.s6_addr, host->h_addr, host->h_length);
    memcpy(&ss, &addr6, sizeof(struct sockaddr_in6));
  } else { 
    if (SODEBUG) printf("Resolvemos LO-QUE-SEA con getaddrinfo\n");
    // Empleamos el getaddrinfo como ultimo recurso, para resolver el nombre en LO-QUE-SEA
    bzero(&hints, sizeof(hints));
    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = PF_UNSPEC;
    hints.ai_protocol = 0;
    sprintf(portS, "%hd", port);
    int result;
    if ( (result = getaddrinfo(thehostname, portS, &hints, &res)) < 0) {
      // Ha fallado todo
      printf("Error en getaddrinfo: %s\n", gai_strerror(result));
      return -1;
    } else {
      protocol = res->ai_protocol;
      memcpy(&ss, res->ai_addr, res->ai_addrlen);
      freeaddrinfo(res);
    }
  }
  return 1;

#endif

}

void Address::Gethostname(char *name, size_t len, bool byName) {
  int flags;
  char nametmp[50], servicetmp[50];

  byName = 0;
  if ( byName) {
    flags = NI_NUMERICSERV;
  } else {
    flags = NI_NUMERICHOST | NI_NUMERICSERV;
  }
  switch ( protocol ) {
  case PF_INET: {
    getnameinfo((struct sockaddr*)&ss, sizeof(struct sockaddr_in), nametmp, 50, servicetmp,
		50, flags);
    strncpy(name, nametmp, len);    
    break;
  }
  case PF_INET6: {
    getnameinfo((struct sockaddr*)&ss, sizeof(struct sockaddr_in6), nametmp, 50, servicetmp,
		50, flags);
    strncpy(name, nametmp, len);
    if (SODEBUG) printf("name original: %s\n", name);
    // Puede parecer una chapuza, pero el hecho es que al resolver, las direcciones
    // IPv6 suelen ir acompañadas de '%' y un numerajo: no nos vale, hay que quitarlo!
    char *name2;
    if ( (name2 = strrchr(name, '%')) ) {
      int l1 = strlen(name2);
      int l2 = strlen(name);
      memset(name, 0, len);
      strncpy(name, nametmp, l2 - l1);
      if (SODEBUG) printf("name modificado: %s\n", name);
    }
    break;
  }  
  default: 
    strncpy(name, "<unsupported>", len); 
    break; 
  }
}

int Address::Getport() {
  switch ( protocol ) {
  case PF_INET: 
    return ntohs((reinterpret_cast<struct sockaddr_in*>(&ss))->sin_port);  
  case PF_INET6: 
    return ntohs((reinterpret_cast<struct sockaddr_in6*>(&ss))->sin6_port);  
  default:
    return -1;
  }
}

int Address::Setport(int port) {
  switch ( protocol ) {
  case PF_INET: {
    struct sockaddr_in *inet = reinterpret_cast<struct sockaddr_in*>(&ss);
    inet->sin_port = htons(port);
    return 1;
  }  
  case PF_INET6: {
    struct sockaddr_in6 *inet6 = reinterpret_cast<struct sockaddr_in6*>(&ss);
    inet6->sin6_port = htons(port);
    return 1;
  }
  default:
    return -1;
  }
}

socklen_t Address::Getsize() { 
  switch ( protocol ) {
  case PF_INET: 
    return sizeof(struct sockaddr_in);
  case PF_INET6: 
    return sizeof(struct sockaddr_in6);
  default:
    return 0;
  }
}

int Address::Setsockaddr(struct sockaddr *addr, int protocol) {
  this->protocol = protocol;
  switch ( protocol ) {
  case PF_INET: {
    memcpy(&this->ss, reinterpret_cast<struct sockaddr_in*>(addr), sizeof(struct sockaddr_in));
    return 1;
  }
  case PF_INET6: {
    memcpy(&this->ss, reinterpret_cast<struct sockaddr_in6*>(addr), sizeof(struct sockaddr_in6));
    return 1;
  }
  default: {
    return -1;
  }
  }
}

struct sockaddr *Address::Getsockaddr() {
  struct sockaddr_in *inet;
  struct sockaddr_in6 *inet6;
  switch ( protocol ) {
  case PF_INET: {
    inet = (struct sockaddr_in*)calloc(1, sizeof(struct sockaddr_in));
    memcpy(inet, reinterpret_cast<struct sockaddr_in*>(&ss), sizeof(struct sockaddr_in));
    return (struct sockaddr*)inet;
  }  
  case PF_INET6: {
    inet6 = (struct sockaddr_in6*)calloc(1, sizeof(struct sockaddr_in6));
    memcpy(inet6, reinterpret_cast<struct sockaddr_in6*>(&ss), sizeof(struct sockaddr_in6));
    return (struct sockaddr*)inet6;
  }
  default:
    return NULL;
  }
}


int Address::Getprotocol() {
  return protocol;
}

int Address::Equals(Address *addr2) {
  if (!addr2) {
    return -1;
  }
  if (addr2->Getport() != this->Getport()) {
    return 0;
  }
  if (addr2->Getprotocol() != this->Getprotocol()) {
    return 0;
  }
  return(this->Hostequals(addr2));
}

int Address::Hostequals(Address *addr2) {
  int result;
  char *name, *name2;
  name = (char*)calloc(50, sizeof(char));
  name2 = (char*)calloc(50, sizeof(char));
  if (!addr2) {
    return -1;
  }
  this->Gethostname(name, 50, 1);
  addr2->Gethostname(name2, 50, 1);
  // Por precaución, si son IPv4 del tipo ::ffff:ipv4, quiza al comparar no se vean identicas
  // Asi que lo quitamos
  char aux[256];
  if (sscanf(name, "::ffff:%s", aux)) {
    strncpy(name, aux, 50);
  }
  memset(aux, 0, 256);
  if (sscanf(name2, "::ffff:%s", aux)) {
    strncpy(name2, aux, 50);
  }
  if (SODEBUG) printf("Son iguales %s y %s?\n", name, name2);
  result = !strcmp(name, name2);
  free(name);
  free(name2);
  return result;
}

void Address::Print() {
  char peername[50];
  int port;
  Gethostname(peername, 50, 0);
  port = Getport();
  printf("\n[%s]:%d\n", peername, port);
}

int Address::IsMulticast() {
  switch ( protocol ) {
  case PF_INET: {
    struct sockaddr_in *inet = reinterpret_cast<struct sockaddr_in*>(&ss);
    return ((inet->sin_addr.s_addr&0x000000FF) == 224 );
  }
  case PF_INET6: {
    struct sockaddr_in6 *inet6 = reinterpret_cast<struct sockaddr_in6*>(&ss);
    return ( IN6_IS_ADDR_MULTICAST(&inet6->sin6_addr) );
  }
  default:
    return -1;
  } 
}

#ifdef WIN32

int Address::Getownaddress(char *interface, char *dstbuffer, int buffersize) {

	gethostname(dstbuffer, buffersize);
	return 1;
  //get this machines host name
  char szHostname[256];
  if (gethostname(szHostname, sizeof(szHostname)))
  {
    printf("Failed in call to gethostname, WSAGetLastError returns %d\n", WSAGetLastError());
    return -1;
  }
  //get host information from the host name
  HOSTENT* pHostEnt = gethostbyname(szHostname);
  if (pHostEnt == NULL) {
    printf("Failed in call to gethostbyname, WSAGetLastError returns %d\n", WSAGetLastError());
    return -1;
  }

  //check the length of the IP adress
  if (pHostEnt->h_length != 4)
  {
    printf("IP address returned is not 32 bits !!\n");
    return -1;
  }

  //call the virtual callback function in a loop
  int nAdapter = 0;
  BOOL bContinue = TRUE;
  while (pHostEnt->h_addr_list[nAdapter] && bContinue)
  {
    in_addr address;
    CopyMemory(&address.S_un.S_addr, pHostEnt->h_addr_list[nAdapter], pHostEnt->h_length);
//    bContinue = EnumCallbackFunction(nAdapter, address);
    nAdapter++;
  }
  return 1;
}

int Address::Getanyownaddress6(char *interface, char *dstbuffer, int buffersize) {
  return -1;
}

int Address::Getownaddress6(char *interface, char *dstbuffer, int buffersize, int scope) {
  return -1;
}

int Address::Getownbcastaddress(char *interface, char *dstbuffer, int buffersize) {
  return -1;
}

#else

int Address::Getownaddress(char *interface, char *dstbuffer, int buffersize) {
  struct ifreq ifr; /* points to one interface returned from ioctl */
  int return_val;
  int sockfd;
  
  if (!interface) return 0;
  sockfd = socket( PF_INET, SOCK_DGRAM, 0 );
  if (sockfd == -1) {
    perror( "ERROR: getBroadcastAddr: error en socket " );
    return 0;
  }
  
  strncpy (ifr.ifr_name, interface, sizeof(ifr.ifr_name));
  
  return_val = ioctl( sockfd,SIOCGIFADDR, &ifr );
  close(sockfd);
  if (return_val == 0 ) {
    if (ifr.ifr_broadaddr.sa_family == AF_INET) {
      struct sockaddr_in *sin = (struct sockaddr_in *) &ifr.ifr_addr;
      //      *local_addr = sin->sin_addr.s_addr;
      strncpy(dstbuffer, inet_ntoa(sin -> sin_addr), buffersize);
      return strlen(dstbuffer);
    } else {
      perror( "unsupported family for dest\n");
      return 0;
    }
  } else {
    perror( "Error when getting the IP address from the interface" );
    return 0;
  }
}

int Address::Getanyownaddress6(char *interface, char *dstbuffer, int buffersize) {
  // Por orden, buscaremos una direccion GLOBAL, SITE y LINK
  if (Getownaddress6(interface, dstbuffer, buffersize, IP6_SCOPE_GLOBAL) < 0) {
    if (Getownaddress6(interface, dstbuffer, buffersize, IP6_SCOPE_SITE) < 0) {
      if (Getownaddress6(interface, dstbuffer, buffersize, IP6_SCOPE_LINK) < 0) {
	return -1;
      }
    }
  }
  return 1;
}

int Address::Getownaddress6(char *interface, char *dstbuffer, int buffersize, int scope) {
  int plen, scopeaux, dad_status, if_idx, addrfound = -1;
  char devname[20];
  char addr6p[8][5];
  FILE *f;
  
  if ((f = fopen("/proc/net/if_inet6", "r")) == NULL) {
    perror ("The file /proc/net/if_inet6 doesn't exist. Please check IPv6 support is enabled.");
    return 1;
  }
  while(1) {
    if (feof(f)) {
      //printf("Fin del fichero\n");
      break;
    }
    int nVals;
    nVals = fscanf(f, "%4s%4s%4s%4s%4s%4s%4s%4s %02x %02x %02x %02x %20s\n",
		   addr6p[0], addr6p[1], addr6p[2], addr6p[3],
		   addr6p[4], addr6p[5], addr6p[6], addr6p[7],
		   &if_idx, &plen, &scopeaux, &dad_status, devname);
    //printf("Leidos %d valores\n", nVals);
    if (nVals != 13) {
      perror("Formato del fichero incorrecto\n");
      break;
    }
    //printf("device = %s y el otro %s\n", devname, interface);
    if ( !strcmp (devname, interface)) {
      //printf("Iguales Scope = %d\n", scope);
      if (scopeaux == scope) {
	char aux[40];
	sprintf(aux, "%s:%s:%s:%s:%s:%s:%s:%s",
		addr6p[0], addr6p[1], addr6p[2], addr6p[3],
		addr6p[4], addr6p[5], addr6p[6], addr6p[7]);
	//printf("IPV6=%s\n", aux);
	//convert_string_to_address_v6( aux, local_addr);
	
	Address *addr = new Address(PF_INET6);
	addr->Resolve(aux, 8000);
	addr->Gethostname(dstbuffer, buffersize, 0);
	
	addrfound = 1;
	break;
      }
    }
  }
  fclose(f);
  //char aux[INET6_ADDRSTRLEN];
  //convert_address_to_string_v6( local_addr, aux );
  return addrfound;
}

int Address::Getownbcastaddress(char *interface, char *dstbuffer, int buffersize) {
  struct ifreq  ifr; /* points to one interface returned from ioctl */
  int return_val;
  int sockfd;
  
  if (!interface) return 0;
  sockfd = socket( PF_INET, SOCK_DGRAM, 0 );
  if (sockfd == -1) {
    perror( "ERROR: getBroadcastAddr: error en socket " );
    return 0;
  }
  
  strncpy (ifr.ifr_name, interface, sizeof(ifr.ifr_name));

  return_val = ioctl(sockfd,SIOCGIFBRDADDR, &ifr);
  close(sockfd);

  if (return_val == 0 ) {
    if (ifr.ifr_broadaddr.sa_family == AF_INET) {
      struct sockaddr_in *sin_ptr = (struct sockaddr_in *)
&ifr.ifr_broadaddr;
      //*broadcast_addr = sin_ptr->sin_addr.s_addr;
      /* wanna see it? */
        strncpy(dstbuffer, inet_ntoa(sin_ptr->sin_addr), buffersize);
        return strlen(dstbuffer);
      /* leave return_val set to 0 to return success! */
    } else {
      perror ("Wrong family for broadcast interface");
      return 0;
    }
  } else {
    perror( "Error when getting the broadcast address" );
    return 0;
  }
}

#endif

/***********************************/

Socket *Socket::Create(int domain, int type) {
  return new Socket(domain, type);
}

Socket::Socket(int sockfd) {
  this->sockfd = sockfd;
  if ( sockfd < 0 ) {
    perror("Socket::Socket(int)");
  }
}

Socket::Socket(int domain, int type) {
  sockfd = socket(domain, type, 0);
  if ( sockfd < 0 ) {
    perror("Socket::Socket(int, int)");
  }
}

Socket::Socket(int domain, int type, int protocol) {
  sockfd = socket(domain, type, protocol);
  if ( sockfd < 0 ) {
    perror("Socket::Socket(int, int, int)");
  }
}

Socket::~Socket() {
  if (sockfd) {
    if ( close(sockfd) < 0 ) {
      perror("Socket::~Socket");
    }
  }
}

int Socket::Getfd() {
  return sockfd;
}

#ifndef WIN32

int Socket::EnableMulticast(Address *address) {
  if (!address->IsMulticast()) {
    return -1;
  }
  switch ( address->Getprotocol() ) {
  case PF_INET: {
    struct sockaddr_in *inet = reinterpret_cast<struct sockaddr_in*>(address->Getsockaddr());
    if ( (inet->sin_addr.s_addr&0x000000FF) == 224 ) {
      struct ip_mreq mreq;
      mreq.imr_multiaddr = inet->sin_addr;
      mreq.imr_interface.s_addr = INADDR_ANY;
      if ( setsockopt(sockfd, SOL_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) != 0 ) {
	perror("Join multicast failed in bind\n");
	return -1;
      }
    }
    break;
  }  
  case PF_INET6: {
    struct sockaddr_in6 *inet6 = reinterpret_cast<struct sockaddr_in6*>(address->Getsockaddr());
    if ( IN6_IS_ADDR_MULTICAST(&inet6->sin6_addr) ) {
      struct ipv6_mreq mreq6;
      mreq6.ipv6mr_multiaddr = inet6->sin6_addr;
      mreq6.ipv6mr_interface = 0;	  
      if ( setsockopt(sockfd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq6, sizeof(mreq6)) != 0 ) {
	perror("Join multicast6 failed in bind\n");
	return -1;
      }
      
    }
    break;
  }  
  default:
    return -1;
  }
  return 1;
}

#endif

int Socket::Bind(Address *address) {
  return (bind(sockfd, address->Getsockaddr(), address->Getsize()));
}

int Socket::Getport() {
  struct sockaddr_in sin;
  int len = sizeof(sin);
  if (getsockname(sockfd, (struct sockaddr *)&sin, (socklen_t *)&len) < 0) {
    return -1;
  } else {
    return ntohs(sin.sin_port);
  }
}

int Socket::Sendto(const void *msg, size_t len, int flags, Address *dest_addr) {
  struct sockaddr *sa;
  int res;
  sa = dest_addr->Getsockaddr();
  if (!sa) return 0;

#ifdef WIN32

  res = sendto(sockfd, (const char *)msg, len, flags, sa, dest_addr->Getsize());  

#else

  res = sendto(sockfd, msg, len, flags, sa, dest_addr->Getsize());  

#endif

  free(sa);
  return res;
}

int Socket::Recvfrom(void *buf, size_t len, int flags, Address *from_addr) {
  int result;
  socklen_t fromlen;
  struct sockaddr_in6 fromtmp;

  memset(&fromtmp, 0, sizeof(fromtmp));
  fromlen = from_addr->Getsize();
  result = recvfrom(sockfd, (char *)buf, len, flags, (struct sockaddr*)&fromtmp, &fromlen);  
  switch ( fromlen ) {
  case sizeof(struct sockaddr_in): {
    from_addr->Setsockaddr(reinterpret_cast<struct sockaddr*>(&fromtmp), PF_INET);
    break;
  }
  case sizeof(struct sockaddr_in6): {
    from_addr->Setsockaddr(reinterpret_cast<struct sockaddr*>(&fromtmp), PF_INET6);
    break;
  }
  default:
    return -1;
  }
  return result;
}

int Socket::Listen(int backlog) {
  return(listen(sockfd, backlog));
}

int Socket::Connect(Address *serv_addr) {
  switch ( serv_addr->Getprotocol() ) {
  case PF_INET: {
    if (SODEBUG) printf("Connect to inet\n");
    struct sockaddr_in *inet = reinterpret_cast<struct sockaddr_in*>(serv_addr->Getsockaddr());
    return(connect(sockfd, (struct sockaddr*)inet, sizeof(struct sockaddr_in)));
  }
  case PF_INET6: {
    if (SODEBUG) printf("Connect to inet6\n");
    struct sockaddr_in6 *inet6 = reinterpret_cast<struct sockaddr_in6*>(serv_addr->Getsockaddr());
    return(connect(sockfd, (struct sockaddr*)inet6, sizeof(struct sockaddr_in6)));
  }
  default:
    return -1;
  }
}
 
Socket *Socket::Accept(Address *from_addr) {
  int result = 0;
  socklen_t from_len;
  struct sockaddr_in6 fromtmp;

  if (from_addr) {
    from_len = from_addr->Getsize();
    result = accept(sockfd, (struct sockaddr*)(&fromtmp), &from_len);
    switch ( from_len ) {
    case sizeof(struct sockaddr_in): {
      from_addr->Setsockaddr(reinterpret_cast<struct sockaddr*>(&fromtmp), PF_INET);   
      break;
    }
    case sizeof(struct sockaddr_in6): {
      from_addr->Setsockaddr(reinterpret_cast<struct sockaddr*>(&fromtmp), PF_INET6);   
      break;
    }
    default:
      return NULL;
    }
  } else {
    result = accept(sockfd, NULL, 0);
  }
  if (result < 0) {
    return NULL;
  } else {
    return new Socket(result);
  }
}

void Socket::SetSocketMaxBuffers() {
  int good_res;
  int res_in;
  int res_out;
  socklen_t res_len = sizeof(int);

  getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&good_res, &res_len);  
  res_in = res_out= good_res;
  while((res_in == res_out) && (res_in < 60*1024)) {
    res_in += 4*1024; 
    res_len = sizeof(int);
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&res_in, res_len);
    getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&res_out, &res_len); 
    if(res_out == res_in)
      good_res = res_in;
  }
  res_len = sizeof(int);
  setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&good_res, res_len);
  getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&good_res, &res_len);
  res_in = res_out = good_res;
  while((res_in == res_out) && (res_in < 60*1024)) {
    res_in += 4*1024;
    res_len = sizeof(int);
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&res_in, res_len);
    getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&res_out, &res_len);
    if(res_out == res_in)
      good_res = res_in;
  }
  res_len = sizeof(int);
  setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&good_res, res_len);
}

int Socket::Close() {
  int aux = sockfd;
  sockfd = 0;
  return(close(aux));
}

int Socket::Isfree(int port, int type) {

  struct sockaddr_in testaddr;
  struct linger opt;
  int testsocket;
  int result;
  
  testsocket = socket(AF_INET, type, 0);
  opt.l_onoff = 1;
  opt.l_linger = 0;
  setsockopt(testsocket, SOL_SOCKET, SO_LINGER, (char*)&opt, sizeof(opt));
  testaddr.sin_family = AF_INET;
  testaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  testaddr.sin_port = htons(port);
  if ( bind(testsocket, (struct sockaddr*)&testaddr, sizeof(testaddr)) < 0)
    // El puerto esta ocupado
    result = 0;
  else {
    result = 1;
    close (testsocket);
  }
  return(result);

}

int Socket::Send(unsigned char *buffer, unsigned int buffersize) {
  return write(sockfd, buffer, buffersize);
}

int Socket::Read(unsigned char *buffer, unsigned int buffersize) {
  return read(sockfd, buffer, buffersize);
}

///////////////////////////////

#ifdef WIN32

void InitiateLibrary() {
	WORD wVersionRequested = MAKEWORD(2,0);
	WSADATA wsaData;
	int rc;
	
	rc = WSAStartup(wVersionRequested, &wsaData);
	if (rc) {
		printf("Error\n");
	}
}

#endif
