/*
 * Copyright ©2020 Travis McGaha.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Summer Quarter 2020 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdio.h>       // for snprintf()
#include <unistd.h>      // for close(), fcntl()
#include <sys/types.h>   // for socket(), getaddrinfo(), etc.
#include <sys/socket.h>  // for socket(), getaddrinfo(), etc.
#include <arpa/inet.h>   // for inet_ntop()
#include <netdb.h>       // for getaddrinfo()
#include <errno.h>       // for errno, used by strerror()
#include <string.h>      // for memset, strerror()
#include <iostream>      // for std::cerr, etc.

#include "./ServerSocket.h"

extern "C" {
  #include "libhw1/CSE333.h"
}

namespace hw4 {

static const int kBufSize = 1024;

ServerSocket::ServerSocket(uint16_t port) {
  port_ = port;
  listen_sock_fd_ = -1;
}

ServerSocket::~ServerSocket() {
  // Close the listening socket if it's not zero.  The rest of this
  // class will make sure to zero out the socket if it is closed
  // elsewhere.
  if (listen_sock_fd_ != -1)
    close(listen_sock_fd_);
  listen_sock_fd_ = -1;
}

bool ServerSocket::BindAndListen(int ai_family, int *listen_fd) {
  // Use "getaddrinfo," "socket," "bind," and "listen" to
  // create a listening socket on port port_.  Return the
  // listening socket through the output parameter "listen_fd".

  // STEP 1:  adapted from class bind_listen example
  bool isValidFamily = ai_family == AF_INET || ai_family == AF_INET6 ||
                       ai_family == AF_UNSPEC;
  if (!isValidFamily) {  // Not valid ai_family
    return false;  // can't create socket to listen
  }
  struct addrinfo hints;  // for getaddrinfo()
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = ai_family;  // make to what user specified
  hints.ai_socktype = SOCK_STREAM;  // stream
  hints.ai_flags = AI_PASSIVE;  // use wildcard "INADDR_ANY"
  hints.ai_flags |= AI_V4MAPPED;  // use v4-mapped v6 if no v6 found
  hints.ai_protocol = IPPROTO_TCP;  // tcp protocol
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;

  std::string portnum = std::to_string(port_);  // get portnum as string
  struct addrinfo *result;  // for address structures
  int res = getaddrinfo(nullptr, portnum.c_str(), &hints, &result);
  if (res != 0) {  // getting addrinfo failed
    return false;
  }  // got addresses, so proceed to find one to bind to
  int listen_val = -1;
  for (struct addrinfo *rp = result; rp != nullptr; rp = rp->ai_next) {
    listen_val = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (listen_val == -1) {  // failed to create this socket
      listen_val = 0;
      continue;
    }  // created socket, so configure binding option ASAP after an exit
    int optval = 1;
    setsockopt(listen_val, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (bind(listen_val, rp->ai_addr, rp->ai_addrlen) == 0) {  // bind worked
      sock_family_ = rp->ai_family;
      break;
    }  // bind failed, so close socket and try next address/port
    close(listen_val);
    listen_val = -1;
  }
  freeaddrinfo(result);  // free struct from getaddrinfo()
  if (listen_val <= 0) {  // if failed to bind, return failure
    return false;
  }  // else, success.  Tell OS that this is a listening socket
  if (listen(listen_val, SOMAXCONN) != 0) {  // failed listening socket
    close(listen_val);
    return false;
  }  // else, we've created a listening socket so set & return output
  listen_sock_fd_ = listen_val;
  *listen_fd = listen_val;
  return true;
}

bool ServerSocket::Accept(int *accepted_fd,
                          std::string *client_addr,
                          uint16_t *client_port,
                          std::string *client_dnsname,
                          std::string *server_addr,
                          std::string *server_dnsname) {
  // Accept a new connection on the listening socket listen_sock_fd_.
  // (Block until a new connection arrives.)  Return the newly accepted
  // socket, as well as information about both ends of the new connection,
  // through the various output parameters.

  // STEP 2:  adapted from class server_accept
  struct sockaddr_storage caddr;
  socklen_t caddr_len = sizeof(caddr);
  int client_fd;
  struct sockaddr *addr = reinterpret_cast<struct sockaddr *>(&caddr);
  while (1) {
    client_fd = accept(listen_sock_fd_, addr, &caddr_len);
    if (client_fd < 0) {
      if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
        continue;
      }  // else, failed on accepting so exit
      return false;
    }  // we've found the connection so unblock
    break;
  }  // now we can process and store the newly accepted socket's info
  *accepted_fd = client_fd;
  if (addr->sa_family == AF_INET) {  // address is IPv4
    char astring[INET_ADDRSTRLEN];  // address string
    struct sockaddr_in *in4 = reinterpret_cast<struct sockaddr_in *>(addr);
    inet_ntop(AF_INET, &(in4->sin_addr), astring, INET_ADDRSTRLEN);
    *client_addr = std::string(astring);
    *client_port = ntohs(in4->sin_port);
  } else {  // address is IPv6
    char astring[INET6_ADDRSTRLEN];  // address string
    struct sockaddr_in6 *in6 = reinterpret_cast<struct sockaddr_in6 *>(addr);
    inet_ntop(AF_INET6, &(in6->sin6_addr), astring, INET6_ADDRSTRLEN);
    *client_addr = std::string(astring);
    *client_port = ntohs(in6->sin6_port);
  }
  char hostname[kBufSize];  // get for storing client_dnsname
  Verify333(getnameinfo(addr, caddr_len, hostname, kBufSize, nullptr, 0, 0)
                        == 0);
  *client_dnsname = std::string(hostname);
  char hname[kBufSize];  // now, we get server info
  hname[0] = '\0';
  if (sock_family_ == AF_INET) {  // server is IPv4
    struct sockaddr_in srvr;
    socklen_t srvrlen = sizeof(srvr);
    char addrbuf[INET_ADDRSTRLEN];
    getsockname(client_fd, (struct sockaddr *) &srvr, &srvrlen);
    inet_ntop(AF_INET, &srvr.sin_addr, addrbuf, INET_ADDRSTRLEN);
    getnameinfo((const struct sockaddr *) &srvr, srvrlen, hname,
                kBufSize, nullptr, 0, 0);  // get server's dns name
    *server_addr = std::string(addrbuf);
  } else {  // server is using IPv6
    struct sockaddr_in6 srvr;
    socklen_t srvrlen = sizeof(srvr);
    char addrbuf[INET6_ADDRSTRLEN];
    getsockname(client_fd, (struct sockaddr *) &srvr, &srvrlen);
    inet_ntop(AF_INET6, &srvr.sin6_addr, addrbuf, INET6_ADDRSTRLEN);
    getnameinfo((const struct sockaddr *) &srvr, srvrlen, hname,
                kBufSize, nullptr, 0, 0);
    *server_addr = std::string(addrbuf);
  }
  *server_dnsname = std::string(hname);  // save server dns name
  return true;  // we've been able to save all needed output.. DONE!!
}

}  // namespace hw4
