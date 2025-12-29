#include "hash_table.h"

#include <stdlib.h>
#include <string.h>

size_t hash(void *restrict buf, size_t buf_size) {
  size_t padding =
      (sizeof(size_t) - (buf_size % sizeof(size_t))) % sizeof(size_t);
  size_t size = buf_size + padding;
  assert(size % sizeof(size_t) == 0);

  size_t *buffer = malloc(size);
  memset(buffer, 0, size);
  memcpy(buffer, buf, buf_size);

  size_t hash_value = 0;
  for (size_t i = 0; i < size / sizeof(size_t); i++) {
    hash_value ^= buffer[i];
  }

  free(buffer);

  return hash_value;
}