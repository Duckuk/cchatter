#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#define ASSERT_HASH_TABLE_TYPES(table, key_type, element_type)                 \
  assert(table->key_size == sizeof(key_type));                                 \
  assert(table->element_size == sizeof(element_type));

struct HashTable {
  // PRIVATE DO NOT TOUCH
  void **_buffer;
  // READ-ONLY
  size_t capacity;
  // READ-ONLY
  size_t len;
  // READ-ONLY
  size_t key_size;
  // READ-ONLY
  size_t element_size;
  // PRIVATE DO NOT TOUCH
  size_t (*_hash_func)(void *restrict, size_t);
};

// Generate a hash from variable-length buf using homemade block-xoring.
size_t hash(void *restrict buf, size_t buf_size);

// Randomly generate a bunch of data and hash it.
double calculate_collision_rate();

// Initialize new hash table with default capacity (1024).
// Return 0 on success, otherwise returns -1
void hash_table_new(struct HashTable *restrict table, size_t key_size,
                    size_t element_size);
// Set `key` to `element` in `table` via `memcpy`.
// Return 0 on success, otherwise returns -1
void hash_table_set(struct HashTable *restrict table, void *restrict key,
                    void *restrict element);
// Set `key` to `element` in `table` via direct assignment to pointer.
// Return 0 on success, otherwise returns -1
void hash_table_set_ptr(struct HashTable *restrict table, void *restrict key,
                        void *restrict ptr);
// Delete `key` from `table`.
// Return 0 on success, otherwise returns -1
[[nodiscard]]
int hash_table_remove(struct HashTable *restrict table, void *restrict key);
// Get `key` from `table`.
// Return 0 on success, otherwise returns `NULL`
[[nodiscard]]
void *hash_table_get(struct HashTable *restrict table, void *restrict key);

size_t hash_table_length(struct HashTable *restrict table);

void hash_table_free(struct HashTable *restrict table);

#endif