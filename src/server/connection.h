#ifndef SERVER_CONNECTION_H

#define SERVER_CONNECTION_H

#include "../../include/hash_table.h"
#include "../../include/packet.h"

#include <sys/socket.h>

struct Connection {
  ConnectionID id;
  struct sockaddr_storage socket_address;
  int socket_fd;
};

struct ConnectionTable {
  struct HashTable table_by_id;
  struct HashTable table_by_fd;
};

// Creates a new connection table.
// Returns 0 on success, otherwise returns -1
[[nodiscard]]
int connection_table_new(struct ConnectionTable *table);
// Adds a new connection `conn` to `table` via `memcpy`.
// Returns 0 on success, otherwise returns -1
[[nodiscard]]
int connection_table_add(struct ConnectionTable *table,
                         struct Connection *conn);
// Removes a connection `conn` from `table`.
// Returns 0 on success, otherwise returns -1
[[nodiscard]]
int connection_table_remove(struct ConnectionTable *table,
                            struct Connection *conn);
// Gets a connection `conn` by id.
// Returns 0 on success, otherwise returns `NULL`
[[nodiscard]]
struct Connection *connection_table_get_by_id(struct ConnectionTable *table,
                                              ConnectionID id);
// Gets a connection `conn` by fd.
// Returns 0 on success, otherwise returns `NULL`
[[nodiscard]]
struct Connection *connection_table_get_by_fd(struct ConnectionTable *table,
                                              int fd);
void connection_table_free(struct ConnectionTable *table);

#endif