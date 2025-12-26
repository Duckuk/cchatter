#include "socket.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// Generic function to get socket port from either host or peer.
// `func` should be either `getsockname` or `getpeername`.
int get_socket_port_generic(int socket_fd,
                            int (*func)(int, struct sockaddr *restrict,
                                        socklen_t *restrict)) {
  struct sockaddr storage;
  socklen_t storage_len = sizeof(storage);
  if (func(socket_fd, &storage, &storage_len) == -1) {
    return -1;
  }

  if (storage.sa_family == AF_INET) {
    struct sockaddr_in *sin = (struct sockaddr_in *)(&storage);
    return sin->sin_port;
  }

  if (storage.sa_family == AF_INET6) {
    struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&storage;
    return sin6->sin6_port;
  }

  errno = EAFNOSUPPORT;
  return -1;
}

// Generic function to get socket ip from either host or peer.
// `func` should be either `getsockname` or `getpeername`.
int get_socket_ip_generic(int socket_fd, char *restrict buf, size_t buf_len,
                          int (*func)(int, struct sockaddr *restrict,
                                      socklen_t *restrict)) {
  struct sockaddr storage;
  socklen_t storage_len = sizeof(storage);
  if (func(socket_fd, &storage, &storage_len) == -1) {
    return -1;
  }

  if (storage.sa_family == AF_INET) {
    struct sockaddr_in *sin = (struct sockaddr_in *)(&storage);
    inet_ntop(AF_INET, (void *)&sin->sin_addr, buf, buf_len);
    return 0;
  }

  if (storage.sa_family == AF_INET6) {
    struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&storage;
    inet_ntop(AF_INET6, (void *)&sin6->sin6_addr, buf, buf_len);
    return 0;
  }

  errno = EAFNOSUPPORT;
  return -1;
}

// Returns a socket fd that connects to the given ip address and port or -1 if
// there was an error
int connect_client_socket(char *restrict ip_address, char *restrict port) {
  struct addrinfo req;
  memset(&req, 0, sizeof req);
  req.ai_family = AF_UNSPEC;
  req.ai_socktype = SOCK_STREAM;
  req.ai_flags = 0;

  struct addrinfo *addr;
  int err = getaddrinfo(ip_address, port, &req, &addr);
  if (err != 0) {
    fprintf(stderr, "getaddrinfo: %s", gai_strerror(err));
    return -1;
  }

  int socket_fd = -1;
  struct addrinfo *addr_p;
  for (addr_p = addr; addr_p != NULL; addr_p = addr_p->ai_next) {
    socket_fd =
        socket(addr_p->ai_family, addr_p->ai_socktype, addr_p->ai_protocol);
    if (socket_fd == -1) {
      continue;
    }

    if (connect(socket_fd, addr_p->ai_addr, addr_p->ai_addrlen) != 0) {
      close(socket_fd);
      continue;
    }

    break;
  }

  freeaddrinfo(addr);

  if (addr_p == NULL) {
    close(socket_fd);
    errno = EADDRNOTAVAIL;
    return -1;
  }

  return socket_fd;
}

// Returns a socket fd that is connected to the given ip address and port,
// otherwise returns -1 and sets errno.
// Note that `listen` will need to be called separately.
int bind_server_socket(char *restrict ip_address, char *restrict port) {
  struct addrinfo req;
  memset(&req, 0, sizeof req);
  req.ai_family = AF_UNSPEC;
  req.ai_socktype = SOCK_STREAM;
  req.ai_flags = AI_PASSIVE;

  struct addrinfo *addr;
  int err = getaddrinfo(ip_address, port, &req, &addr);
  if (err != 0) {
    fprintf(stderr, "getaddrinfo: %s", gai_strerror(err));
    errno = EADDRNOTAVAIL;
    return -1;
  }

  int socket_fd = -1;
  struct addrinfo *addr_p;
  for (addr_p = addr; addr_p != NULL; addr_p = addr_p->ai_next) {
    socket_fd =
        socket(addr_p->ai_family, addr_p->ai_socktype, addr_p->ai_protocol);
    if (socket_fd == -1) {
      continue;
    }

    if (bind(socket_fd, addr_p->ai_addr, addr_p->ai_addrlen) != 0) {
      close(socket_fd);
      continue;
    }

    break;
  }

  freeaddrinfo(addr);

  if (addr_p == NULL) {
    close(socket_fd);
    errno = EADDRNOTAVAIL;
    return 1;
  }

  return socket_fd;
}

// Returns 0 on success and puts the ip in buf, otherwise returns -1 and sets
// errno
int get_socket_ip(int socket_fd, char *restrict buf, size_t buf_len) {
  return get_socket_ip_generic(socket_fd, buf, buf_len, getsockname);
}

// Returns the socket's port on success, otherwise returns -1 and sets errno
int get_socket_port(int socket_fd) {
  return get_socket_port_generic(socket_fd, getsockname);
}

// Returns 0 on success and puts the ip in buf, otherwise returns -1 and sets
// errno
int get_socket_peer_ip(int socket_fd, char *restrict buf, size_t buf_len) {
  return get_socket_ip_generic(socket_fd, buf, buf_len, getpeername);
}

// Returns the port of the socket's peer on success, otherwise returns -1 and
// sets errno
int get_socket_peer_port(int socket_fd) {
  return get_socket_port_generic(socket_fd, getpeername);
}
