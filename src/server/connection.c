#include "connection.h"
#include <string.h>

void connection_table_new(struct ConnectionTable *table) {
  memset(table, 0, sizeof *table);
  hash_table_new(&table->table_by_id, sizeof(ConnectionID),
                 sizeof(struct Connection));

  hash_table_new(&table->table_by_fd, sizeof(int), sizeof(struct Connection));
}

void connection_table_add(struct ConnectionTable *table,
                          struct Connection *conn) {
  hash_table_set(&table->table_by_id, conn->id, conn);

  void *ptr = hash_table_get(&table->table_by_id, conn->id);

  hash_table_set_ptr(&table->table_by_fd, &conn->socket_fd, ptr);
}

void connection_table_set_id(struct ConnectionTable *table,
                             struct Connection *conn, ConnectionID new_id) {
  ConnectionID old_id;
  memset(old_id, 0, sizeof old_id);
  memcpy(old_id, conn->id, sizeof old_id);

  memcpy(conn->id, new_id, sizeof conn->id);

  hash_table_move(table, old_id, new_id);
}

int connection_table_remove(struct ConnectionTable *table,
                            struct Connection *conn) {
  if (hash_table_remove(&table->table_by_id, conn->id) == -1) {
    return -1;
  }

  if (hash_table_remove(&table->table_by_fd, &conn->socket_fd) == -1) {
    return -1;
  }

  return 0;
}

struct Connection *connection_table_get_by_id(struct ConnectionTable *table,
                                              ConnectionID id) {
  return hash_table_get(&table->table_by_id, id);
}

struct Connection *connection_table_get_by_fd(struct ConnectionTable *table,
                                              int fd) {
  return hash_table_get(&table->table_by_fd, &fd);
}

void connection_table_free(struct ConnectionTable *table) {
  hash_table_free(&table->table_by_id);
  hash_table_free(&table->table_by_fd);
}