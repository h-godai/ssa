/* TSSocket.h  -*-C++-*-
//
//
//
// CopyRight(C) 2005 Technicalarts h.godai
//
*/

#ifndef TS_TSSOCKET_H
#define TS_TSSOCKET_H

#include <map>
#include <vector>
#include <string>

#include "../../Common/PlatformType.hpp"

#ifdef	WIN32

#ifndef _MT
#define _MT
#endif
#ifndef _AFXDLL
#include	<windows.h>
#endif
#include	<process.h>
#include	<Winsock.h>
#pragma comment(lib, "wsock32.lib")
typedef unsigned long in_addr_t;
typedef int socklen_t;
  
#endif	// WIN32

#ifdef	TS_PLATFORM_LINUX

#include	<pthread.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netinet/tcp.h>
#include	<sys/time.h>
#include	<sys/poll.h>
#include	<sys/resource.h>
#include	<unistd.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<fcntl.h>
#include	<ctype.h>
#include	<signal.h>
#include	<time.h>


typedef int SOCKET;

#ifndef INVALID_SOCKET
const SOCKET INVALID_SOCKET = -1;
#endif

#endif	// LINUX

namespace ts {


  // OSに依存しないSocketクラス
  class Socket {
  public:
    enum DomainType {
      Local = AF_LOCAL,
      IP    = AF_INET,
      IP6   = AF_INET6
    };
    enum SocketType {
      Dgram = SOCK_DGRAM,
      Stream = SOCK_STREAM,
    };

    struct DisconnectException {};
    
    Socket(SOCKET fd=INVALID_SOCKET) : mFD(fd) {
#ifdef	WIN32
      WSADATA	wsaData;
      WORD	ver;
      ver = MAKEWORD( 2, 2 );
      WSAStartup( ver, &wsaData );
#endif
    }

    Socket(DomainType domain, SocketType type, int protocol=0) {
      mFD = open(domain,type,protocol);
    }
    
    ~Socket() {
    }

    SOCKET open(DomainType domain, SocketType type, int protocol=0) {
      mFD = ::socket(domain, type, protocol);
      return mFD;
    }


    Socket& operator=(const Socket& s) {
      mFD = s.mFD;
      return *this;
    }

    bool valid() const {
      return mFD != INVALID_SOCKET;
    }
    
    

    SOCKET bind(struct sockaddr *my_addr, socklen_t addrlen) {
      return ::bind(mFD, my_addr, addrlen);
    }

    SOCKET listen(int backlog) {
      return ::listen(mFD, backlog);
    }
    Socket accept(struct sockaddr* addr, socklen_t* addrlen) {
      SOCKET fd = ::accept(mFD, addr, addrlen);
      return Socket(fd);
    }

    SOCKET select(int n, fd_set *readfds, fd_set* writefds, fd_set *exceptfds, struct timeval *timeout) {
      return ::select(n, readfds, writefds, exceptfds, timeout);
    }
    int select(fd_set *readfds, fd_set* writefds, fd_set *exceptfds, struct timeval *timeout) {
      return ::select(int(mFD+1), readfds, writefds, exceptfds, timeout);
    }
      
    size_t recv(char* buf, int len, int flags) {
      return ::recv(mFD, buf, len, flags);
    }

    int recvFrom(char* buf, int len, int flags, struct sockaddr* from, socklen_t* fromlen) {
      return ::recvfrom(mFD, buf, len, flags, from, fromlen);
    }

    int send(const char* msg, int len, int flags) {
      return ::send(mFD, msg, len, flags);
    }
    int sendTo(const void* msg, size_t len, int flags, const struct sockaddr* to, socklen_t tolen) {
      return ::sendto(mFD, msg, len, flags, to, tolen);
    }
    
    int shutDown(int how) {
      return ::shutdown(mFD, how);
    }

    int setSockOpt(int level, int optname, const char* optval, int optlen) {
      return ::setsockopt(mFD, level, optname, optval, optlen);
    }

    int getSockOpt(int level, int optname, char * optval, socklen_t *optlen) {
      return ::getsockopt(mFD, level, optname, optval, optlen);
    }
    
    void fDset(fd_set* fd) {
      FD_SET(mFD, fd);
    }

    int connect( const struct sockaddr *addr, socklen_t addrlen) {
      return :: connect(mFD, addr, addrlen);   
    }


    void close() {
      if (mFD != INVALID_SOCKET) {
#ifdef	WIN32
	::closesocket(mFD);
#else
	::close(mFD);
#endif
	mFD = INVALID_SOCKET;
      }
    }

    // Ethernet-IFのMACアドレスを列挙する
    typedef std::map<std::string, uint64_t> MacAddressList_t;
    static void EnumMacAddress(MacAddressList_t& macList);



    //  private:
    SOCKET mFD;
  };

}




#endif
