#ifndef SOCKET_H
#define SOCKET_H

#include <stddef.h>

// Returns a socket fd that connects to the given ip address and port or -1 if
// there was an error
[[nodiscard]]
int connect_client_socket(char *restrict ip_address, char *restrict port);

// Returns a socket fd that is connected to the given ip address and port,
// otherwise returns -1 and sets errno.
// Note that `listen` will need to be called separately.
[[nodiscard]]
int bind_server_socket(char *restrict ip_address, char *restrict port);

// Returns 0 on success and puts the ip in buf, otherwise returns -1 and sets
// errno
[[nodiscard]]
int get_socket_ip(int socket_fd, char *restrict buf, size_t buf_len);

// Returns the socket's port on success, otherwise returns -1 and sets errno
[[nodiscard]]
int get_socket_port(int socket_fd);

// Returns 0 on success and puts the ip in buf, otherwise returns -1 and sets
// errno
[[nodiscard]]
int get_socket_peer_ip(int socket_fd, char *restrict buf, size_t buf_len);

// Returns the port of the socket's peer on success, otherwise returns -1 and
// sets errno
[[nodiscard]]
int get_socket_peer_port(int socket_fd);

#endif