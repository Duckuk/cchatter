#ifndef SOCKET_H
#define SOCKET_H

#include <stddef.h>

[[nodiscard]]
int connect_client_socket(char *restrict ip_address, char *restrict port);
[[nodiscard]]
int bind_server_socket(char *restrict ip_address, char *restrict port);

[[nodiscard]]
int get_socket_ip(int socket_fd, char *restrict buf, size_t buf_len);
[[nodiscard]]
int get_socket_port(int socket_fd);

[[nodiscard]]
int get_socket_peer_ip(int socket_fd, char *restrict buf, size_t buf_len);
[[nodiscard]]
int get_socket_peer_port(int socket_fd);

#endif